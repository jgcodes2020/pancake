/**
 * @file dwarf.hpp
 * @author your name (you@domain.com)
 * @brief Helper functions around libdwarf, nicer exception handling
 * @version 0.1
 * @date 2021-06-14
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_DWARF_HPP_
#define _PANCAKE_DWARF_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <memory>

namespace dwarf {
  #include <libdwarf.h>
  #include <dwarf.h>
  
  using std::string;
  
  /**
   * @brief Throwable version of Dwarf_Error.
   */
  class dwarf_error_t : public std::runtime_error {
  public:
    dwarf_error_t(const char* what):
      std::runtime_error(what) {}
    dwarf_error_t(const string& what):
      std::runtime_error(what) {}
    /**
     * @brief Constructs a throwable error, then deallocates the provided error struct.
     * 
     * @param dbg the associated instance, for deallocation
     * @param what the error struct to use
     */
    dwarf_error_t(Dwarf_Debug dbg, Dwarf_Error what):
      std::runtime_error(dwarf_errmsg(what)) {
      dwarf_dealloc_error(dbg, what);
    }
  };
  
  /**
   * @brief Throws a dwarf_error_t if the provided return code is an error.
   * 
   * @param dbg the associated instance, for deallocation 
   * @param rcode the return code from the libdwarf function
   * @param err the error struct, used to construct the error
   * @param strict If true, throws on any return code that isn't DW_DLV_OK
   */
  inline void check_error(Dwarf_Debug dbg, int rcode, Dwarf_Error err, bool strict) {
    if (rcode == DW_DLV_ERROR)
      throw dwarf_error_t(dbg, err);
    else if ((rcode != DW_DLV_OK) && strict)
      throw dwarf_error_t("DWARF error: unexpected return code");
  }
  
  /**
   * @brief Clean wrapper over dwarf_init_path().
   * 
   * @param path 
   * @return Dwarf_Debug 
   */
  inline Dwarf_Debug init(const char* path) {
    Dwarf_Debug dbg;
    Dwarf_Error err;
    int rcode = dwarf_init_path(
      path, nullptr, 0,
      DW_DLC_READ, DW_GROUPNUMBER_ANY,
      nullptr, nullptr, &dbg,
      nullptr, 0, 0,
      &err
    );
    check_error(dbg, rcode, err, true);
    return dbg;
  }
  inline Dwarf_Debug init(string path) {
    return dwarf::init(path.c_str());
  }
  
  /**
   * @brief Clean wrapper over dwarf_finish().
   * 
   * @param dbg 
   */
  inline void close(Dwarf_Debug dbg) {
    Dwarf_Error err;
    int rcode = dwarf_finish(dbg, &err);
    check_error(dbg, rcode, err, true);
  }
  
  struct dwarf_cu_info {
    Dwarf_Unsigned 
      cu_header_len, 
      abbrev_offset, 
      type_offset, 
      next_cu_header;
    Dwarf_Half
      version_stamp,
      address_size,
      offset_size,
      extension_size,
      header_cu_type;
    Dwarf_Sig8
      signature;
    bool valid = true;
  };
  /**
   * @brief Advances to the compile unit, returning metadata as a struct.
   * 
   * @param dbg the instance to advance
   * @param is_info If true uses the info section, otherwise uses the types section.
   * @return A struct containing compile-unit information. If there are no more, the "valid" flag on the struct is set to false.
   */
  inline dwarf_cu_info next_cu_header(Dwarf_Debug dbg, bool is_info) {
    dwarf_cu_info result;
    Dwarf_Error err;
    int rcode = dwarf_next_cu_header_d(
      dbg, is_info,
      &(result.cu_header_len), &(result.version_stamp),
      &(result.abbrev_offset), &(result.address_size),
      &(result.offset_size), &(result.extension_size),
      &(result.signature), &(result.type_offset),
      &(result.next_cu_header), &(result.header_cu_type),
      &err
    );
    check_error(dbg, rcode, err, false);
    if (rcode == DW_DLV_NO_ENTRY)
      result.valid = false;
    return result;
  }
  
  
}
#endif