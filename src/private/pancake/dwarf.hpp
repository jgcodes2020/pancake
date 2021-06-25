/**
 * @file dwarf.hpp
 * @author jgcodes2020
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
#include <filesystem>

#include "pancake/dwarf_enums.hpp"

namespace dwarf {
  #include <libdwarf.h>
  
  /********************
  C EXTENSION LIBRARY
  *******************/
  inline void dwarf_dealloc_string(Dwarf_Debug dbg, char* string) {
    dwarf_dealloc(dbg, string, DW_DLA_STRING);
  }
  
  using std::string;
  
  /********************
  C++ UTILITY LIBRARY
  *******************/
  
  /**
   * @brief Throwable version of Dwarf_Error.
   */
  class dw_error : public std::runtime_error {
  public:
    dw_error(const char* what):
      std::runtime_error(what) {}
    dw_error(const string& what):
      std::runtime_error(what) {}
    /**
     * @brief Constructs a throwable error, then deallocates the provided error struct.
     * 
     * @param dbg the associated instance, for deallocation
     * @param what the error struct to use
     */
    dw_error(Dwarf_Debug dbg, Dwarf_Error what):
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
    if (rcode == DW_DLV::ERROR_)
      throw dw_error(dbg, err);
    else if ((rcode != DW_DLV::OK) && strict)
      throw dw_error("DWARF error: unexpected return code");
  }
  
  class dw_global {
    friend class dw_global_list;
    friend class dw_die;
  private:
    const Dwarf_Debug m_dbg;
    Dwarf_Global m_glob;
    dw_global(Dwarf_Debug dbg, Dwarf_Global g) :
      m_dbg(dbg), m_glob(g) {}
  public:
    Dwarf_Global operator*() {
      return m_glob;
    }
  
    string name();
  };
  
  class dw_global_list {
    friend class dw_debug;
  private:
    const Dwarf_Debug m_dbg;
    Dwarf_Global* m_list;
    const size_t m_size;
    
    dw_global_list(Dwarf_Debug dbg, Dwarf_Global* list, size_t size) :
      m_dbg(dbg), m_list(list), m_size(size) {}
  public:
    /**
     * @brief Iterator over global list.
     * @note The iterator will refuse to move its index value past the array.
     */
    class iterator;
    
    ~dw_global_list() {
      dwarf_globals_dealloc(m_dbg, m_list, m_size);
    }
    
    Dwarf_Global* operator*() {
      return m_list;
    }
    
    iterator begin();
    iterator end();
    
    size_t size() const {
      return m_size;
    }
    
    dw_global operator[](size_t index) const {
      if (index >= m_size) {
        std::stringstream fmt;
        fmt << "Index " << index << " out of bounds for size " << m_size;
        throw std::out_of_range(fmt.str());
      }
      return dw_global(m_dbg, m_list[index]);
    }
  };
  
  class dw_debug {
  private:
    const Dwarf_Debug m_dbg;
  public:
    /**
     * @brief Initializes a Dwarf_Debug instance using dwarf_init_path().
     * 
     * @param path the executable file/shared library to parse
     */
    dw_debug(const char* path);
    
    ~dw_debug();
    /**
     * @brief Initializes a Dwarf_Debug instance using dwarf_init_path().
     * 
     * @param path the executable file/shared library to parse
     */
    dw_debug(string path) :
      dw_debug(path.c_str()) {}
      
    const Dwarf_Debug& operator*() {
      return m_dbg;
    }
    
    dw_global_list list_globals() const;
  };
  
  class dw_attribute {
    friend class dw_die;
  private:
    const Dwarf_Debug m_dbg;
    const Dwarf_Attribute m_attr;
    dw_attribute(const dw_die& die, DW_AT attr);
  public:
    ~dw_attribute();
    
    DW_FORM get_form();
    
    /**
     * @brief Returns this attribute as a string, if it is a string attribute.
     * 
     * @return the string value of this attribute
     */
    string as_string();
    
    /**
     * @brief Returns this attribute as a referenced DIE, if this attribute is a reference.
     * 
     * @return the DIE referenced by this attribute
     */
    dw_die as_linked_die();
    
    /**
     * @brief Returns this attribute as an unsigned integer, if it is a constant.
     * 
     * @return the unsigned value of this attribute
     */
    uint64_t as_unsigned_int();
    
    /**
     * @brief Returns this attribute as a signed integer, if it is a constant.
     * 
     * @return the signed value of this attribute.
     */
    int64_t as_signed_int();
  };
  
  class dw_die {
    friend class dw_attribute;
  private:
    Dwarf_Debug m_dbg;
    Dwarf_Die m_die;
    
    dw_die(Dwarf_Debug dbg, Dwarf_Die die):
      m_dbg(dbg), m_die(die) {}
  public:
    dw_die(dw_global g);
    ~dw_die();
    
    Dwarf_Die operator*() {
      return m_die;
    }
    
    dw_die& operator=(const dw_die& that) {
      m_dbg = that.m_dbg;
      m_die = that.m_die;
      return *this;
    }
    dw_die& operator=(dw_die&& that) {
      m_dbg = std::move(that.m_dbg);
      m_die = std::move(that.m_die);
      return *this;
    }
    
    /**
     * @brief Returns this DIE's tag.
     * 
     * @return DW_TAG 
     */
    DW_TAG tag();
    
    /**
     * @brief Returns an attribute.
     * 
     * @param name the attribute to look for
     * @return dw_attribute 
     * @exception std::invalid_argument if the attribute is not found
     * @exception dwarf::dw_error if there was an error during retrieval
     */
    dw_attribute attr(DW_AT name);
    
    /**
     * @brief Returns true if this die has the requested attribute.
     * 
     * @param name the attribute to look for
     * @return true if this die has the requested attribute
     * @return false if this die does not have the requested attribute.
     */
    bool has_attr(DW_AT name);
  };
}
#endif