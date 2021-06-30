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

#include <vcruntime_string.h>
#include <vector>
#include <windows.h>

#include <LIEF/LIEF.hpp>
#include <nlohmann/json.hpp>

#include "pancake/exparser.hpp"
#include "pancake/macro_defns.hpp"
#include "pancake/utils.hpp"
#include "pancake/exception.hpp"
#include "pancake/dwarf.hpp"

using namespace std;
using namespace dwarf;
using namespace LIEF;
using namespace nlohmann;

namespace fs = std::filesystem;

using pancake::exparser::opcode;

template<typename T>
static T dll_get(HMODULE handle, string name) {
  return reinterpret_cast<T>(
    GetProcAddress(handle, name.c_str())
  );
}


namespace pancake {
  
  struct sm64::impl {
  public:
    HMODULE dll;
    dw_debug dbg;
    string path;
    
    impl(string path) : 
      dll(LoadLibraryW(strcvt::to_utf16(path).c_str())),
      dbg(dw_debug(path)),
      path(path) {
      dll_get<void(*)(void)>(dll, "sm64_init")();
    }
    
    ~impl() {
      FreeLibrary(dll);
    }
    
    void* const get(string expr) {
      auto ast = exparser::parse(exparser::preprocess(expr));
      
      uint8_t* ptr = dll_get<uint8_t*>(dll, ast.global);
      dw_die die = dbg.list_globals()[ast.global];
      // resolve incomplete DIE
      if (die.has_attr(DW_AT::specification)) {
        die = die
          .attr(DW_AT::specification).as_linked_die();
      }
      die = die
        .attr(DW_AT::type).as_linked_die();
      
      for (auto& in : ast.instructions) {
        cerr << "current DIE tag is: " << die.tag() << endl;
        cerr << "current instruction is: " << in.first << endl << endl;
        
        if (die.tag() == DW_TAG::typedef_) {
          die = die
            .attr(DW_AT::type).as_linked_die();
        }
        
        switch (in.first) {
          case opcode::SUBSCRIPT: {
            size_t index = any_cast<size_t>(in.second);
            DW_TAG die_tag = die.tag();
            if (die_tag != DW_TAG::array_type && die_tag != DW_TAG::pointer_type) {
              stringstream fmt;
              fmt << "Failed evaluating expression " << expr << ": expected array or pointer";
              throw type_error(fmt.str());
            }
            
            // find array element size and push pointer forward
            die = die
              .attr(DW_AT::type).as_linked_die();
            size_t size;
            if (die.tag() == DW_TAG::typedef_) {
              size = die
                .attr(DW_AT::type).as_linked_die()
                .attr(DW_AT::byte_size).as_unsigned_int();
            }
            else {
              size = die
                .attr(DW_AT::byte_size).as_unsigned_int();
            }
            
            ptr += (size * index);
          } break;
          case opcode::MEMBER: {
            string member = any_cast<string>(in.second);
            if (die.tag() == DW_TAG::pointer_type) {
              die = die
                .attr(DW_AT::type).as_linked_die();
            }
            if (die.tag() == DW_TAG::typedef_) {
              die = die
                .attr(DW_AT::type).as_linked_die();
            }
            switch (die.tag()) {
              case DW_TAG::structure_type: {
                dw_die field_die = die.first_child();
                {
                  while (field_die) {
                    if (field_die.attr(DW_AT::name).as_string() == member)
                      goto past_struct_search;
                    field_die = field_die.sibling();
                  }
                  
                  stringstream fmt;
                  fmt << "Failed evaluating expression " << expr << ": ";
                  fmt << "Could not find struct member " << member;
                  throw invalid_argument(fmt.str());
                }
                past_struct_search:
                size_t offset = field_die
                  .attr(DW_AT::data_member_location).as_unsigned_int();
                
                die = field_die
                  .attr(DW_AT::type).as_linked_die();
                
                if (die.tag() == DW_TAG::typedef_)
                  die = die
                    .attr(DW_AT::type).as_linked_die();
                
                ptr += offset;
              } break;
              case DW_TAG::union_type: {
                // don't offset pointer, but change DIE
                dw_die field_die = die.first_child();
                {
                  while (field_die) {
                    if (field_die.attr(DW_AT::name).as_string() == member)
                      goto past_union_search;
                    field_die = field_die.sibling();
                  }
                  
                  stringstream fmt;
                  fmt << "Failed evaluating expression " << expr << ": ";
                  fmt << "Could not find union member " << member;
                  throw invalid_argument(fmt.str());
                }
                past_union_search:
                die = field_die
                  .attr(DW_AT::type).as_linked_die();
                
                if (die.tag() == DW_TAG::typedef_)
                  die = die
                    .attr(DW_AT::type).as_linked_die();
              } break;
              default: {
                stringstream fmt;
                fmt << "Failed evaluating expression " << expr << ": expected struct or union (actual: " << die.tag() << ")";
                throw type_error(fmt.str());
              }
            }
          } break;
          default: {
            throw std::logic_error("INTERNAL ERROR: this should not happen, report this as a bug.");
          }
        }
      }
      
      if (die.tag() == DW_TAG::typedef_) {
        die = die
          .attr(DW_AT::type).as_linked_die();
      }
      
      if (die.tag() != DW_TAG::base_type) {
        stringstream fmt;
        fmt << "Failed evaluating expression " << expr << ": ";
        fmt << "Accessor does not refer to a field of primitive type";
        throw incomplete_accessor(fmt.str());
      }
      
      return reinterpret_cast<void*>(ptr);
    }
    
    void advance() {
      dll_get<void(*)(void)>(dll, "sm64_update")();
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