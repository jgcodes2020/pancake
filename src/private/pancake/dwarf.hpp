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
#include <optional>

#include "pancake/dwarf_enums.hpp"

namespace dwarf {
  #include <libdwarf.h>
  
  /********************
  C EXTENSION LIBRARY
  *******************/
  inline void dwarf_dealloc_string(Dwarf_Debug dbg, char* string) {
    dwarf_dealloc(dbg, string, DW_DLA_STRING);
  }
  
  /********************
  C++ UTILITY LIBRARY
  *******************/
  
  class dw_die;
  
  /**
   * @brief Throwable version of Dwarf_Error.
   */
  class dw_error : public std::runtime_error {
  public:
    dw_error(const char* what):
      std::runtime_error(what) {}
    dw_error(const std::string& what):
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
    if (rcode == DW_DLV::ERROR)
      throw dw_error(dbg, err);
    else if ((rcode != DW_DLV::OK) && strict)
      throw dw_error("DWARF error: unexpected return code");
  }
  
  class dw_global {
    friend class dw_global_list;
    friend class dw_die;
  private:
    const Dwarf_Debug m_dbg;
    const Dwarf_Global m_glob;
    dw_global(Dwarf_Debug dbg, Dwarf_Global g) :
      m_dbg(dbg), m_glob(g) {}
  public:
    Dwarf_Global operator*() {
      return m_glob;
    }
  
    std::string name();
    
    dw_die as_die();
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
    
    ~dw_global_list() {
      dwarf_globals_dealloc(m_dbg, m_list, m_size);
    }
    
    Dwarf_Global* operator*() {
      return m_list;
    }
    
    size_t size() const {
      return m_size;
    }
    
    /**
     * @brief Acquires a specific property.
     * 
     * @param name 
     * @return dw_global 
     */
    dw_global operator[](std::string name) const;
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
    dw_debug(std::string path) :
      dw_debug(path.c_str()) {}
    
    dw_global_list list_globals() const;
  };
  
  class dw_attribute {
    friend class dw_die;
    friend class dw_attribute_list;
  private:
    const Dwarf_Debug m_dbg;
    const Dwarf_Attribute m_attr;
    const bool m_needs_dealloc;
    dw_attribute(Dwarf_Debug dbg, Dwarf_Attribute attr, bool dealloc = true) :
      m_dbg(dbg), m_attr(attr), m_needs_dealloc(dealloc) {}
  public:
    ~dw_attribute() {
      if (m_needs_dealloc) dwarf_dealloc_attribute(m_attr);
    }
    
    DW_FORM form();
    
    DW_AT name();
    
    /**
     * @brief Returns this attribute as a string, if it is a string attribute.
     * 
     * @return the string value of this attribute
     */
    std::string as_string();
    
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
  
  class dw_attribute_list {
    friend class dw_die;
  private:
    const Dwarf_Debug m_dbg;
    const Dwarf_Attribute* m_attrs;
    const size_t m_size;
    dw_attribute_list(Dwarf_Debug dbg, Dwarf_Attribute* attrs, size_t size) :
      m_dbg(dbg), m_attrs(attrs), m_size(size) {}
  public:
    ~dw_attribute_list();
    
    size_t size() const {
      return m_size;
    }
    
    dw_attribute operator[](size_t index) const;
  };
  
  class dw_die {
    friend class dw_attribute;
    friend class dw_global;
  private:
    Dwarf_Debug m_dbg;
    Dwarf_Die m_die;
    bool m_dealloc;
  public:
    dw_die(Dwarf_Debug dbg, Dwarf_Die die, bool dealloc = true):
      m_dbg(dbg), m_die(die), m_dealloc(dealloc) {}
    dw_die(dw_global g);
  public:
    dw_die(uintptr_t off);
    ~dw_die() {
      if (m_dealloc && (m_die != nullptr))
        dwarf_dealloc_die(m_die);
    }
    // disable copy construction/assignment
    dw_die& operator=(dw_die const& other) = delete;
    dw_die(dw_die const& other) {
      m_dbg = other.m_dbg;
      m_die = other.m_die;
      m_dealloc = false;
    }
    
    // allow move construction/assignment
    dw_die& operator=(dw_die&& other) {
      std::swap(m_dbg, other.m_dbg);
      std::swap(m_die, other.m_die);
      m_dealloc = true;
      return *this;
    }
    dw_die(dw_die&& other) {
      std::swap(m_dbg, other.m_dbg);
      std::swap(m_die, other.m_die);
      m_dealloc = true;
    }
    
    /**
     * @brief Checks whether this die is a null DIE.
     * 
     * @return true if this die is null
     * @return false if this die is not null
     */
    bool is_null() {
      return (m_dbg == nullptr || m_die == nullptr);
    }
    /**
     * @brief The inverse of `dw_die::is_null()`.
     * 
     * @return true if this die is not null
     * @return false if this die is null
     */
    operator bool () {
      return !(m_dbg == nullptr || m_die == nullptr);
    }
    
    
    /**
     * @brief Returns this DIE's tag.
     * 
     * @return this DIE's tag
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
    
    /**
     * @brief Lists the attributes on this DIE.
     * 
     * @return dw_attribute_list a list of attributes.
     */
    dw_attribute_list list_attrs();
    
    /**
     * @brief Returns the first child of this DIE, if available.
     * 
     * @return A DIE representing the child if it exists, otherwise, it is marked as a NULL die.
     */
    dw_die first_child();
    
    /**
     * @brief Returns the next sibling of this DIE, if available.
     * 
     * @return A DIE representing the sibling if it exists, otherwise, it is marked as a NULL die.
     */
    dw_die sibling();
  };
}
#endif