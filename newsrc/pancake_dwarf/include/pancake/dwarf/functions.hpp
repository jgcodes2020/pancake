/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DWARF_FUNCTIONS_HPP_
#define _PANCAKE_DWARF_FUNCTIONS_HPP_
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <pancake/dwarf/enums.hpp>
#include <pancake/dwarf/error.hpp>
#include <pancake/dwarf/memory.hpp>
#include <libdwarf/libdwarf.h>

namespace pdwarf {
  namespace details {
    template<typename T>
    struct is_shared_ptr : public std::false_type {};
    
    template<typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : public std::true_type {};
    
    template<typename T>
    struct is_unique_ptr : public std::false_type {};
    
    template<typename T, typename D>
    struct is_unique_ptr<std::unique_ptr<T, D>> : public std::true_type {};
    
    template<typename T>
    const bool is_shared_ptr_v = is_shared_ptr<T>::value;
    
    template<typename T>
    const bool is_unique_ptr_v = is_unique_ptr<T>::value;
    
    template<typename T>
    T as_ptr(const std::enable_if_t<std::is_pointer_v<T>, T>& x) {
      return x;
    }
    template<typename T>
    decltype(std::declval<T>().get()) as_ptr(const T& x) {
      return x.get();
    }
    
    template<typename... Ts>
    struct false_sink : public std::false_type {};
    
    template<typename... Ts>
    const bool false_sink_v = false_sink<Ts...>::value;
  }
  
  using dw_signed = Dwarf_Signed;
  
  inline Dwarf_Debug init_path(const char* path) {
    Dwarf_Debug dbg;
    Dwarf_Error err;
    
    int rcode = dwarf_init_path(
      // Path
      path,
      // True path return values, no MacOS support as of yet 
      nullptr, 0, 0, 
      // const Debug& handler, nope
      nullptr, nullptr, 
      // Actual return values
      &dbg, &err
    );
    switch (rcode) {
      case DW_DLV_ERROR: {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      } break;
      case DW_DLV_NO_ENTRY: {
        std::stringstream fmt;
        fmt << path << " is not a valid object file";
        
        throw file_not_found(fmt.str());
      }
    }
    
    return dbg;
  }
  inline Dwarf_Debug init_path(std::string& path) {
    return init_path(path.c_str());
  }
  
  template<typename Debug>
  Dwarf_Global* get_globals(const Debug& dbg_p, dw_signed& size) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    
    Dwarf_Global* globals;
    Dwarf_Error err;
    
    int rcode = dwarf_get_globals(dbg, &globals, &size, &err);
    switch (rcode) {
      case DW_DLV_ERROR: {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      } break;
      case DW_DLV_NO_ENTRY: {
        throw no_pubnames_section("Executable has no .dwarf_pubnames section");
      } break;
    }
    
    return globals;
  }
  
  template<typename Global, typename Debug>
  Dwarf_Die global_die(const Global& global_p, const Debug& debug_p) {
    Dwarf_Global global = details::as_ptr<Global>(global_p);
    Dwarf_Debug dbg = details::as_ptr<Debug>(debug_p);
    
    Dwarf_Error err;
    int rcode;
    
    Dwarf_Off offset;
    rcode = dwarf_global_die_offset(global, &offset, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    
    std::cerr << "Global DIE offset: " << offset << "\n";
    
    Dwarf_Die die;
    rcode = dwarf_offdie_b(dbg, offset, true, &die, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    
    std::cerr << "DIE pointer: " << die << "\n";
    
    return die;
  }
  
  template<typename Global, typename Debug>
  std::string global_name(const Global& global_p, const Debug& debug_p) {
    Dwarf_Global global = details::as_ptr<Global>(global_p);
    Dwarf_Debug dbg = details::as_ptr<Debug>(debug_p);
    
    Dwarf_Error err;
    
    char* name;
    int rcode = dwarf_globname(global, &name, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    return name;
  }
  
  template<typename Die, typename Debug>
  bool has_attr(const Die& die_p, pdwarf::attr_type type, const Debug& dbg_p) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    Dwarf_Die die = details::as_ptr<Die>(die_p);
    
    Dwarf_Error err;
    
    Dwarf_Bool result;
    int rcode = dwarf_hasattr(die, static_cast<uint16_t>(type), &result, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    return result;
  }
  
  template<typename R, typename Die, typename Debug>
  R attr(const Die& die_p, pdwarf::attr_type type, const Debug& dbg_p) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    Dwarf_Die die = details::as_ptr<Die>(die_p);
    
    Dwarf_Error err;
    
    Dwarf_Attribute attr;
    int rcode = dwarf_attr(die, Dwarf_Half(type), &attr, &err);
    switch (rcode) {
      case DW_DLV_ERROR: {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      } break;
      case DW_DLV_NO_ENTRY: {
        std::stringstream fmt;
        fmt << "DIE does not have attribute " << type;
        
        throw std::invalid_argument(fmt.str());
      }
    }
    
    if constexpr (std::is_same_v<R, std::string>) {
      char* result;
      rcode = dwarf_formstring(attr, &result, &err);
      if (rcode != DW_DLV_OK) {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      }
      return result;
    }
    else if constexpr (std::is_same_v<R, Dwarf_Die>) {
      Dwarf_Off off;
      rcode = dwarf_global_formref(attr, &off, &err);
      if (rcode != DW_DLV_OK) {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      }
      
      Dwarf_Die result;
      rcode = dwarf_offdie_b(dbg, off, true, &result, &err);
      if (rcode != DW_DLV_OK) {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      }
      
      return result;
    }
    else if constexpr (std::is_same_v<R, Dwarf_Unsigned>) {
      Dwarf_Unsigned result;
      rcode = dwarf_formudata(attr, &result, &err);
      if (rcode != DW_DLV_OK) {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      }
      
      return result;
    }
    else if constexpr (std::is_same_v<R, Dwarf_Signed>) {
      Dwarf_Signed result;
      rcode = dwarf_formsdata(attr, &result, &err);
      if (rcode != DW_DLV_OK) {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      }
      
      return result;
    }
    else {
      static_assert(details::false_sink<R>::value, "attr requires R to be one of {std::string, Dwarf_Die, Dwarf_Unsigned, Dwarf_Signed}");
    }
    
  }
  
  template<typename Die, typename Debug>
  Dwarf_Die child(const Die& die_p, const Debug& dbg_p) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    Dwarf_Die die = details::as_ptr<Die>(die_p);
    
    Dwarf_Error err;
    
    Dwarf_Die result;
    int rcode = dwarf_child(die, &result, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    
    return result;
  }
  
  template<typename Die, typename Debug>
  Dwarf_Die siblingof(const Die& die_p, const Debug& dbg_p) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    Dwarf_Die die = details::as_ptr<Die>(die_p);
    
    Dwarf_Error err;
    
    Dwarf_Die result;
    int rcode = dwarf_siblingof_b(dbg, die, true, &result, &err);
    switch (rcode) {
      case DW_DLV_ERROR: {
        int code = dwarf_errno(err);
        dwarf_dealloc_error(dbg, err);
        
        throw std::system_error(code, dwarf_category());
      } break;
      case DW_DLV_NO_ENTRY: {
        return nullptr;
      }
    }
    
    return result;
  }
  
  template<typename Die, typename Debug>
  die_tag tag(const Die& die_p, const Debug& dbg_p) {
    Dwarf_Debug dbg = details::as_ptr<Debug>(dbg_p);
    Dwarf_Die die = details::as_ptr<Die>(die_p);
    
    Dwarf_Error err;
    
    Dwarf_Half result;
    int rcode = dwarf_tag(die, &result, &err);
    if (rcode != DW_DLV_OK) {
      int code = dwarf_errno(err);
      dwarf_dealloc_error(dbg, err);
      
      throw std::system_error(code, dwarf_category());
    }
    
    return die_tag(result);
  }
}
#endif