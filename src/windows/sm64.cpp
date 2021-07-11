#include "pancake/sm64.hpp"

#include <any>
#include <cstdint>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <locale>
#include <regex>

#include <unordered_map>
#include <variant>
#include <vcruntime_string.h>
#include <vector>
#include <windows.h>

#include <LIEF/LIEF.hpp>
#include <nlohmann/json.hpp>

#include "pancake/expr/parse.hpp"
#include "pancake/expr/compile.hpp"
#include "pancake/macro_defns.hpp"
#include "pancake/overload.hpp"
#include "pancake/utils.hpp"
#include "pancake/exception.hpp"
#include "pancake/dwarf.hpp"

using namespace std;
using namespace dwarf;
using namespace LIEF;
using namespace nlohmann;

namespace fs = std::filesystem;

using namespace pancake::expr;

using rvaddress_t = uintptr_t;

template<typename T = void*>
static T get_proc_address(HMODULE handle, string name) {
  return reinterpret_cast<T>(
    GetProcAddress(handle, name.c_str())
  );
}
static rvaddress_t get_proc_rva(HMODULE handle, string name) {
  return static_cast<rvaddress_t>(
    get_proc_address<uint8_t*>(handle, name) - reinterpret_cast<uint8_t*>(handle)
  );
}
template<typename T = void*>
static T rva_to_pointer(HMODULE handle, rvaddress_t rva) {
  return reinterpret_cast<T>(
    reinterpret_cast<uint8_t*>(handle) + rva
  );
}
static rvaddress_t pointer_to_rva(HMODULE handle, void* ptr) {
  return reinterpret_cast<uint8_t*>(ptr) - reinterpret_cast<uint8_t*>(handle);
}


namespace pancake {
  
  struct sm64::impl {
    HMODULE dll;
    dw_debug dbg;
    string path;
    
    std::unordered_map<string, expr_eval> cache;
    
    impl(string path) : 
      dll(LoadLibraryW(strcvt::to_utf16(path).c_str())),
      dbg(dw_debug(path)),
      path(path) {
      get_proc_address<void(*)(void)>(dll, "sm64_init")();
    }
    
    ~impl() {
      FreeLibrary(dll);
    }
    
    void* const get(string expr) {
      
      expr_eval eval;
      auto search = cache.find(expr);
      if (search != cache.end()) {
        eval = search->second;
      }
      else {
        eval = compile(
          parse(preprocess(expr)),
        dbg);
        cache.insert(pair<string, expr_eval>(expr, eval));
      }
      
      rvaddress_t ptr = get_proc_rva(dll, eval.global);
      cerr << eval << "\n";
      for (size_t i = 0; i < eval.steps.size(); i++) {
        visit(overload {
          [&](const expr_eval::offset& step) mutable -> void {
            ptr += step.off;
          },
          [&](const expr_eval::indirect& step) mutable -> void {
            if (ptr == 0) {
              stringstream fmt;
              fmt << "Expression \033[0;38;5;202m" << expr;
              fmt << "\033[0m attempts to indirect a null pointer";
              fmt << " (step " << i << ")";
              throw invalid_argument(fmt.str());
            }
            uint8_t* next_ptr = *rva_to_pointer<uint8_t**>(dll, ptr);
            ptr = pointer_to_rva(dll, next_ptr);
          }
        }, eval.steps[i]);
      }
      return rva_to_pointer(dll, ptr);
    }
    
    void advance() {
      get_proc_address<void(*)(void)>(dll, "sm64_update")();
    }
  };
  
  struct sm64::savestate::impl {
    HMODULE game_ptr;
    struct region {
      uintptr_t address;
      uintptr_t size;
    };
    array<region, 2> save_sections;
    array<vector<uint8_t>, 2> buffers;
    
    impl(sm64 const& game) {
      unique_ptr<PE::Binary> bin = PE::Parser::parse(game.pimpl->path);
      
      PE::Section section;
      // .data
      section = bin->get_section(".data");
      save_sections[0] = region {
        section.virtual_address(),
        section.virtual_size()
      };
      // .bss
      section = bin->get_section(".bss");
      save_sections[1] = region {
        section.virtual_address(),
        section.virtual_size()
      };
      
      game_ptr = game.pimpl->dll;
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        buffers[i] = vector<uint8_t>();
        buffers[i].reserve(save_sections[i].size);
      }
    }
    
    void save(sm64 const& game) {
      if (game_ptr != game.pimpl->dll)
        throw domain_error("Savestate does not belong to this instance of SM64");
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        memmove(&buffers[i], (game.pimpl->dll + save_sections[i].address), save_sections[i].size);
      }
    }
    void load(sm64& game) {
      if (game_ptr != game.pimpl->dll)
        throw domain_error("Savestate does not belong to this instance of SM64");
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        memmove((game.pimpl->dll + save_sections[i].address), &buffers[i], save_sections[i].size);
      }
    }
  };
  
  sm64::savestate::savestate(sm64 const& game) {
    pimpl = unique_ptr<impl>(new impl(game));
  }
  void sm64::savestate::save(sm64 const& game) {
    pimpl->save(game);
  }
  void sm64::savestate::load(sm64& game) const {
    pimpl->load(game);
  }
  
  sm64::sm64(fs::path path) :
    sm64(path.string()) {}
  
  sm64::sm64(string path) :
    pimpl(new impl(path)) {}
    
  sm64::sm64(const char* path) :
    sm64(string(path)) {}
  
  void* const sm64::_impl_get(string expr) {
    return pimpl->get(expr);
  }
  
  sm64::savestate sm64::alloc_svst() {
    return savestate(*this);
  }
  
  void sm64::save_svst(savestate& st) {
    st.save(*this);
  }
  
  void sm64::load_svst(const savestate& st) {
    st.load(*this);
  }
  
  void sm64::advance() {
    pimpl->advance();
  }
  
  any sm64::_impl_constant(string name) {
    json constants = sm64_macro_defns::get()["constants"];
    auto match = constants.find(name);
    if (match == constants.end()) {
      stringstream fmt;
      fmt << name << " is not a constant";
      throw domain_error(name);
    }
    auto obj = *match;
    string o_type = obj["type"];
    if (o_type == "s64") {
      return make_any<int64_t>(static_cast<int64_t>(obj["value"]));
    }
    else if (o_type == "f64") {
      return make_any<double>(static_cast<double>(obj["value"]));
    }
    else if (o_type == "void") {
      return make_any<nullptr_t>(nullptr);
    }
    stringstream fmt;
    fmt << name << " is not a constant";
    throw domain_error(name);
  }
}