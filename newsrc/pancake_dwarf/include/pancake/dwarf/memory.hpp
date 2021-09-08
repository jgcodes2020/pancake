/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DWARF_MEMORY_HPP_
#define _PANCAKE_DWARF_MEMORY_HPP_
#include <libdwarf/libdwarf.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
namespace pdwarf {
  // make it unusable unless specialized
  template<typename T>
  struct dwarf_deleter;
  
  template<>
  struct dwarf_deleter<Dwarf_Debug> {
    void operator()(Dwarf_Debug ptr) noexcept {
      Dwarf_Error err;
      // if there are exceptions, stop immediately
      int rcode = dwarf_finish(ptr, &err);
      if (rcode != DW_DLV_OK) {
        std::cerr << "Error closing a Dwarf_Debug: " << dwarf_errmsg(err) << "\n";
        std::cerr << "Terminating to prevent further damage...";
        std::terminate();
      }
    }
  };
  
  template<>
  struct dwarf_deleter<Dwarf_Die> {
    void operator()(Dwarf_Die ptr) noexcept {
      dwarf_dealloc_die(ptr);
    }
  };
  
  template<>
  struct dwarf_deleter<Dwarf_Error> {
  private:
    std::shared_ptr<Dwarf_Debug_s> m_dbg;
  public:
    dwarf_deleter(const std::shared_ptr<Dwarf_Debug_s>& dbg) :
      m_dbg(dbg) {}
    
    void operator()(Dwarf_Error ptr) noexcept {
      dwarf_dealloc_error(m_dbg.get(), ptr);
    }
  };
  
  template<>
  struct dwarf_deleter<Dwarf_Global[]> {
  private:
    std::shared_ptr<Dwarf_Debug_s> m_dbg;
    Dwarf_Signed m_size;
  public:
    dwarf_deleter(const std::shared_ptr<Dwarf_Debug_s>& dbg, Dwarf_Signed size) :
      m_dbg(dbg), m_size(size) {}
    
    void operator()(Dwarf_Global* ptr) noexcept {
      dwarf_globals_dealloc(m_dbg.get(), ptr, m_size);
    }
  };
  
  /*
   * Constructs a unique pointer using dwarf_deleter.
   * The variadic arguments are forwarded to the constructor of the deleter.
   */
  template<typename T, typename... Args>
  std::unique_ptr<
    std::remove_pointer_t<T>, 
    dwarf_deleter<T>
  > dwarf_make_unique(std::enable_if_t<!std::is_array_v<T>, T> v, Args&&... args) {
    return std::unique_ptr<std::remove_pointer_t<T>, dwarf_deleter<T>>(v, dwarf_deleter<T>(std::forward<Args>(args)...));
  }
  
  template<typename T, typename... Args>
  std::unique_ptr<
    T, 
    dwarf_deleter<T>
  > dwarf_make_unique(std::remove_extent_t<std::enable_if_t<std::is_array_v<T>, T>>* v, Args&&... args) {
    return std::unique_ptr<T, dwarf_deleter<T>>(v, dwarf_deleter<T>(std::forward<Args>(args)...));
  }
  
  /**
   * @brief A non-owning smart pointer.
   * @details It allows implicit conversion from standard types.
   * 
   * @tparam T 
   */
  template<typename T>
  struct nos_pointer {
    T* ptr;
    
    nos_pointer(T* src) : ptr(src) {}
    nos_pointer(std::shared_ptr<T> src) : ptr(src.get()) {}
    template<typename D>
    nos_pointer(std::unique_ptr<T, D> src) : ptr(src.get()) {}
    
    operator T* () {
      return ptr;
    }
  };
  
  template<typename T>
  using nosp_t = nos_pointer<std::remove_pointer_t<T>>;
}
#endif