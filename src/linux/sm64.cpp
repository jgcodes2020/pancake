#include <pancake/sm64.hpp>

#include <any>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <locale>
#include <regex>

#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <dlfcn.h>

#include <LIEF/LIEF.hpp>
#include <nlohmann/json.hpp>

#include <pancake/expr/parse.hpp>
#include <pancake/expr/compile.hpp>
#include <pancake/macro_defns.hpp>
#include <pancake/stx/overload.hpp>
#include <pancake/utils/strcvt.hpp>
#include <pancake/exception.hpp>
#include <pancake/dwarf/functions.hpp>

using namespace std;
using namespace LIEF;
using namespace nlohmann;

namespace fs = std::filesystem;

using namespace pancake::expr;

using rvaddress_t = uintptr_t;

template<typename T = void*>
static T get_proc_address(void* handle, const string& name) {
  static_assert(std::is_pointer<T>::value, "OOOF");
  
  void* fp = dlsym(handle, name.c_str());
  if (!fp) {
    stringstream fmt;
    fmt << "dlsym threw error: " << dlerror();
  }
  
  return reinterpret_cast<T>(fp);
}


namespace pancake {
  
  struct sm64::impl {
    void* so;
    std::shared_ptr<Dwarf_Debug_s> dbg;
    string path;
    
    std::unordered_map<string, void*> cache;
    
    impl(string path) : 
      so(dlopen(path.c_str(), RTLD_LAZY)),
      dbg(pdwarf::dwarf_make_shared<Dwarf_Debug>(pdwarf::init_path(path))),
      path(path) {
      get_proc_address<void(*)(void)>(so, "sm64_init")();
    }
    
    ~impl() {
      dlclose(so);
    }
    
    void* const get(string expr) {
      
      expr_eval eval;
      auto search = cache.find(expr);
      if (search != cache.end()) {
        return search->second;
      }
      else {
        eval = compile(
          parse(expr),
        dbg);
      }
      
      uint8_t* ptr = get_proc_address<uint8_t*>(so, eval.global);
       cerr << eval << "\n";
      for (size_t i = 0; i < eval.steps.size(); i++) {
        visit(stx::overload {
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
            ptr = *reinterpret_cast<uint8_t**>(ptr);
          }
        }, eval.steps[i]);
      }
      cache.emplace(expr, ptr);
      return ptr;
    }
    
    void advance() {
      get_proc_address<void(*)(void)>(so, "sm64_update")();
    }
  };
  
  struct sm64::savestate::impl {
    void* game_ptr;
    struct region {
      uintptr_t address;
      uintptr_t size;
    };
    array<region, 2> save_sections;
    array<vector<uint8_t>, 2> buffers;
    
    impl(sm64 const& game) {
      unique_ptr<ELF::Binary> bin = ELF::Parser::parse(game.pimpl->path);
      
      ELF::Section section;
      // .data
      section = bin->get_section(".data");
      save_sections[0] = region {
        section.virtual_address(),
        section.size()
      };
      // .bss
      section = bin->get_section(".bss");
      save_sections[1] = region {
        section.virtual_address(),
        section.size()
      };
      
      game_ptr = game.pimpl->so;
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        buffers[i] = vector<uint8_t>();
        buffers[i].reserve(save_sections[i].size);
      }
    }
    
    void save(sm64 const& game) {
      if (game_ptr != game.pimpl->so)
        throw domain_error("Savestate does not belong to this instance of SM64");
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        memmove(&buffers[i], (reinterpret_cast<uint8_t*>(game.pimpl->so) + save_sections[i].address), save_sections[i].size);
      }
    }
    void load(sm64& game) {
      if (game_ptr != game.pimpl->so)
        throw domain_error("Savestate does not belong to this instance of SM64");
      
      for (uint_fast8_t i = 0; i < 2; i++) {
        memmove((reinterpret_cast<uint8_t*>(game.pimpl->so) + save_sections[i].address), &buffers[i], save_sections[i].size);
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
  
  sm64::savestate sm64::alloc_svst() const {
    return savestate(*this);
  }
  
  void sm64::save_svst(savestate& st) const {
    st.save(*this);
  }
  
  void sm64::load_svst(const savestate& st) {
    st.load(*this);
  }
  
  void sm64::advance() {
    pimpl->advance();
  }
  
  const variant<double, int64_t, std::nullptr_t> sm64::constant(string name) const {
    json constants = sm64_macro_defns::get()["constants"];
    auto match = constants.find(name);
    if (match == constants.end()) {
      stringstream fmt;
      fmt << name << " is not a constant";
      throw invalid_argument(name);
    }
    json matched = *match;
    string type = matched.at("type");
    if (type == "s64") {
      return variant<double, int64_t, nullptr_t>(
        matched.at("value").get<int64_t>()
      );
    }
    else if (type == "f64") {
      return variant<double, int64_t, nullptr_t>(
        matched.at("value").get<double>()
      );
    }
    else if (type == "null") {
      return variant<double, int64_t, nullptr_t>(nullptr);
    }
    else {
      stringstream fmt;
      fmt << "INTERNAL ERROR: ";
      fmt << name << " was found but its type (" << type << ") was invalid";
      throw runtime_error(fmt.str());
    }
  }
}