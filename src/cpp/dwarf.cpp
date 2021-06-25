#include "pancake/dwarf.hpp"
#include "libdwarf.h"

#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <string.h>

using std::string, std::cout, std::stringstream;

namespace dwarf {
  // dw_global
  
  string dw_global::name() {
    Dwarf_Error err;
    char* cstr;
    int rcode = dwarf_globname(m_glob, &cstr, &err);
    if (rcode != DW_DLV::OK) {
      throw dw_error(m_dbg, err);
    }
    return string(cstr);
  }
  
  // dw_global_list
  
  class dw_global_list::iterator {
  public:
    using value_type = Dwarf_Global;
    using reference = Dwarf_Global&;
    using pointer = Dwarf_Global*;
    using difference_type = ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;
  private:
    Dwarf_Global* list;
    const size_t size;
    size_t index;
  public:
    iterator(size_t start_index, Dwarf_Global* list, size_t size) :
      list(list), size(size), index(start_index) {}
    
    /*******************************
    FORWARD ITERATOR
    ********************************/
    
    // prefix increment
    iterator& operator++() {
      if (index < size) index++;
      return *this;
    }
    // suffix increment
    iterator operator++(int) {
      iterator result = *this;
      if (index < size) index++;
      return result;
    }
    
    // dereference
    reference operator*() {
      return list[index];
    }
    // dereference and get
    pointer operator->() {
      return &list[index];
    }
    
    // equal
    bool operator==(iterator other) {
      return (list == other.list) && (index == other.index);
    }
    // not equal
    bool operator!=(iterator other) {
      return !(operator==(other));
    }
    
    /*******************************
    BIDIRECTIONAL ITERATOR
    ********************************/
    
    // Prefix decrement
    iterator& operator--() {
      if (index > 0) index--;
      return *this;
    }
    
    // Suffix decrement
    iterator operator--(int) {
      iterator result = *this;
      if (index > 0) index--;
      return result;
    }
    
    /*******************************
    RANDOM-ACCESS ITERATOR
    ********************************/
    
    // add and assign
    iterator& operator+=(difference_type n) {
      if (index + n > size) index = size;
      // If n > index, index - n > 0
      else if (n > index) index = 0;
      else index += size;
      return *this;
    }
    // add
    friend iterator operator+(iterator a, difference_type n) {
      iterator result = a;
      a += n;
      return result;
    }
    // add
    friend iterator operator+(difference_type n, iterator a) {
      return operator+(a, n);
    }
    
    // subtract and assign
    iterator& operator-=(difference_type n) {
      return operator+=(-n);
    }
    // subtract
    friend iterator operator-(iterator a, difference_type n) {
      iterator result = a;
      a -= n;
      return result;
    }
    
    // iterator difference
    difference_type operator-(iterator other) {
      difference_type a = static_cast<difference_type>(index);
      difference_type b = static_cast<difference_type>(other.index);
      return a - b;
    }
    
    // subscript
    reference operator[](size_t n) {
      if (index + n > size) 
        throw std::out_of_range("Attempted to access index past the end");
      return list[index + n];
    }
    
    // less
    bool operator<(iterator other) {
      if (list != other.list) throw std::invalid_argument("Iterators do not belong to the same list");
      return index < other.index;
    }
    // more
    bool operator>(iterator other) {
      if (list != other.list) throw std::invalid_argument("Iterators do not belong to the same list");
      return index > other.index;
    }
    // less/equal
    bool operator<=(iterator other) {
      if (list != other.list) throw std::invalid_argument("Iterators do not belong to the same list");
      return index <= other.index;
    }
    // more/equal
    bool operator>=(iterator other) {
      if (list != other.list) throw std::invalid_argument("Iterators do not belong to the same list");
      return index >= other.index;
    }
  };
  
  dw_global_list::iterator dw_global_list::begin() {
    return iterator(0, m_list, m_size);
  }
  dw_global_list::iterator dw_global_list::end() {
    return iterator(m_size, m_list, m_size);
  }
  
  // dw_debug
  
  dw_debug::dw_debug(const char* path) :
    m_dbg([path]() {
      // lambda initializer is op
      Dwarf_Debug dbg;
      Dwarf_Error err;
      int rcode = dwarf_init_path(
        path, nullptr, 0,
        DW_DLC_READ, DW_GROUPNUMBER_ANY,
        nullptr, nullptr, &dbg,
        nullptr, 0, 0,
        &err
      );
      if (rcode != DW_DLV::OK) {
        std::stringstream fmt;
        fmt << "Error occurred during init: " << dwarf_errmsg(err);
        dwarf_dealloc_error(dbg, err);
        throw dw_error(fmt.str());
      }
      return dbg;
    }()) {}
  
  dw_debug::~dw_debug() {
    Dwarf_Error err;
    int rcode = dwarf_finish(m_dbg, &err);
    if (rcode != DW_DLV::OK) {
      std::cerr << "Error occurred during finalization: " << dwarf_errmsg(err);
      dwarf_dealloc_error(m_dbg, err);
      std::cerr << "Program will abort *immediately*.";
      std::abort();
    }
  }
  
  dw_global_list dw_debug::list_globals() const{
    Dwarf_Global* list;
    Dwarf_Signed size;
    Dwarf_Error err;
    
    int rcode = dwarf_get_globals(m_dbg, &list, &size, &err);
    if (rcode != DW_DLV::OK) {
      throw dw_error(m_dbg, err);
    }
    
    return dw_global_list(m_dbg, list, size);
  }
  
  // dw_die
  
  dw_die::dw_die(dw_global g) :
    m_dbg(g.m_dbg),
    m_die([this, g]() {
      auto flags = cout.flags();
      
      const Dwarf_Off&& die_offset = [this, g]() {
        Dwarf_Off result;
        Dwarf_Error err;
        int rcode = dwarf_global_die_offset(g.m_glob, &result, &err);
        if (rcode != DW_DLV::OK) {
          throw dw_error(m_dbg, err);
        }
        return result;
      }();
      cout << std::hex << m_die << std::endl;
      return [this, die_offset]() {
        Dwarf_Error err;
        Dwarf_Die die;
        int rcode = dwarf_offdie_b(m_dbg, die_offset, true, &die, &err);
        if (rcode != DW_DLV::OK) {
          throw dw_error(m_dbg, err);
        }
        auto flags = cout.flags();
        cout.flags(flags);
        return die;
      }();    
    }()) {
    }
  
  dw_die::~dw_die() {
    dwarf_dealloc_die(m_die);
  }
  
  dw_attribute dw_die::attr(DW_AT name) {
    return dw_attribute(*this, name);
  }
  
  bool dw_die::has_attr(DW_AT name) {
    Dwarf_Bool x;
    Dwarf_Error err;
    
    cout << std::hex << m_die << std::endl;
    
    int rcode = dwarf_hasattr(m_die, static_cast<Dwarf_Half>(name), &x, &err);
    if (rcode != DW_DLV::OK) {
      throw dw_error(m_dbg, err);
    }
    return x;
  }
  
  DW_TAG dw_die::tag() {
    Dwarf_Half c_tag;
    Dwarf_Error err;
    int rcode = dwarf_tag(m_die, &c_tag, &err);
    if (rcode != DW_DLV::OK) {
      throw dw_error(m_dbg, err);
    }
    return static_cast<DW_TAG>(c_tag);
  }
  
  // dw_attribute
  
  dw_attribute::dw_attribute(const dw_die& die, DW_AT name) :
    m_dbg(die.m_dbg), m_attr([this, die, name]() -> Dwarf_Attribute {
      Dwarf_Error err;
      Dwarf_Attribute attr;
      ((void) *(die.m_die));
      cout << '(' << ((std::intptr_t) die.m_die) << ", " << static_cast<uint16_t>(name) << ")\n";
      int rcode = dwarf_attr(die.m_die, static_cast<uint16_t>(name), &attr, &err);
      if (rcode == DW_DLV::NO_ENTRY) {
        std::stringstream fmt;
        fmt << "DIE does not contain attribute " << std::hex << static_cast<uint16_t>(name);
        throw std::invalid_argument(fmt.str());
      }
      else if (rcode == DW_DLV::ERROR_) {
        throw dw_error(m_dbg, err);
      }
      return attr;
    }()) {}
  
  dw_attribute::~dw_attribute() {
    dwarf_dealloc_attribute(m_attr);
  }
  
  DW_FORM dw_attribute::get_form() {
    Dwarf_Error err;
    
    uint16_t form;
    int rcode = dwarf_whatform(m_attr, &form, &err);
    if (rcode == DW_DLV::ERROR_) {
      throw dw_error(m_dbg, err);
    }
    
    return static_cast<DW_FORM>(form);
  }
  
  string dw_attribute::as_string() {
    Dwarf_Error err;
    
    char* cstr;
    int rcode = dwarf_formstring(m_attr, &cstr, &err);
    if (rcode == DW_DLV::ERROR_) {
      throw dw_error(m_dbg, err);
    }
    
    string result = string(cstr);
    dwarf_dealloc_string(m_dbg, cstr);
    return result;
  }
  
  dw_die dw_attribute::as_linked_die() {
    Dwarf_Error err; int rcode;
    
    Dwarf_Off offset;
    rcode = dwarf_global_formref(m_attr, &offset, &err);
    if (rcode == DW_DLV::ERROR_) {
      throw dw_error(m_dbg, err);
    }
    
    dw_die result(m_dbg, [this, offset]() -> Dwarf_Die {
      Dwarf_Die result; Dwarf_Error err;
      int rcode = dwarf_offdie_b(m_dbg, offset, false, &result, &err);
      if (rcode == DW_DLV::ERROR_) {
        throw dw_error(m_dbg, err);
      }
      return result;
    }());

    
    
    return result;
  }
  
  uint64_t dw_attribute::as_unsigned_int() {
    Dwarf_Error err;
    
    uint64_t result;
    int rcode = dwarf_formudata(m_attr, &result, &err);
    if (rcode == DW_DLV::ERROR_) {
      throw dw_error(m_dbg, err);
    }
    
    return result;
  }
  
  int64_t dw_attribute::as_signed_int() {
    Dwarf_Error err;
    
    int64_t result;
    int rcode = dwarf_formsdata(m_attr, &result, &err);
    if (rcode == DW_DLV::ERROR_) {
      throw dw_error(m_dbg, err);
    }
    
    return result;
  }
}