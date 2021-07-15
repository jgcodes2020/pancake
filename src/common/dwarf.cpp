#include <pancake/dwarf.hpp>

#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <string.h>

#include <libdwarf/libdwarf.h>
#include <pancake/exception.hpp>

using namespace std;

static void check_not_null(void* ptr, string msg) {
  if (ptr == nullptr)
    throw pancake::null_pointer_exception(msg);
}

namespace dwarf {
  // dw_global
  
  string dw_global::name() {
    Dwarf_Error err;
    char* cstr;
    int rcode = dwarf_globname(m_glob, &cstr, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    return string(cstr);
  }
  
  dw_die dw_global::as_die() {
    return dw_die(*this);
  }
  
  // dw_global_list
  /*
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
  */
  
  dw_global dw_global_list::operator[](string name) const {
    Dwarf_Global g;
    char* cstr;
    const char* c_name = name.c_str();
    Dwarf_Error err;
    int rcode;
    
    for (int i = 0; i < m_size; i++) {
      g = m_list[i];
      rcode = dwarf_globname(g, &cstr, &err);
      if (rcode == DW_DLV::ERROR) 
        throw dw_error(m_dbg, err);
      
      if (std::strcmp(cstr, c_name) == 0) {
        return dw_global(m_dbg, g);
      }
    }
    stringstream fmt;
    fmt << name << " was not found";
    throw std::invalid_argument(fmt.str());
  }
  
  // dw_debug
  
  dw_debug::dw_debug(const char* path) :
    m_dbg([path]() {
      // lambda initializer is op
      Dwarf_Debug dbg;
      Dwarf_Error err;
      int rcode = dwarf_init_path(
        path, nullptr, 0, 0,
        nullptr, nullptr,
        &dbg, &err
      );
      if (rcode == DW_DLV::ERROR) {
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
    if (rcode == DW_DLV::ERROR) {
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
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return dw_global_list(m_dbg, list, size);
  }
  
  
  // dw_die
  
  dw_die::dw_die(dw_global g) :
    m_dbg(g.m_dbg),
    m_die([this, g]() {
      
      const Dwarf_Off&& die_offset = [this, g]() {
        Dwarf_Off result;
        Dwarf_Error err;
        int rcode = dwarf_global_die_offset(g.m_glob, &result, &err);
        if (rcode == DW_DLV::ERROR) {
          throw dw_error(m_dbg, err);
        }
        return result;
      }();
      return [this, die_offset]() {
        Dwarf_Error err;
        Dwarf_Die die;
        int rcode = dwarf_offdie_b(m_dbg, die_offset, true, &die, &err);
        if (rcode == DW_DLV::ERROR) {
          throw dw_error(m_dbg, err);
        }
        auto flags = cout.flags();
        cout.flags(flags);
        return die;
      }();    
    }()) {}
  
  dw_die::dw_die(const dw_die& other) {
    m_dbg = other.m_dbg;
    
    Dwarf_Error err;
    int rcode;
    
    Dwarf_Off off;
    rcode = dwarf_dieoffset(other.m_die, &off, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    Dwarf_Die newdie;
    rcode = dwarf_offdie_b(m_dbg, off, true, &newdie, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
  }
  
  dw_attribute dw_die::attr(DW_AT name) {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Error err;
    int rcode;
    
    Dwarf_Attribute attr;
    rcode = dwarf_attr(m_die, static_cast<Dwarf_Half>(name), &attr, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return dw_attribute(m_dbg, attr);
  }
  
  bool dw_die::has_attr(DW_AT name) {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Bool x;
    Dwarf_Error err;
    
    int rcode = dwarf_hasattr(m_die, static_cast<Dwarf_Half>(name), &x, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    return x;
  }
  
  DW_TAG dw_die::tag() {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Half c_tag;
    Dwarf_Error err;
    int rcode = dwarf_tag(m_die, &c_tag, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    return static_cast<DW_TAG>(c_tag);
  }
  
  dw_attribute_list dw_die::list_attrs() {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Attribute* attrs;
    Dwarf_Signed size;
    Dwarf_Error err;
    int rcode = dwarf_attrlist(m_die, &attrs, &size, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    return dw_attribute_list(m_dbg, attrs, static_cast<size_t>(size));
  }
  
  dw_die dw_die::first_child() {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Die child;
    Dwarf_Error err;
    int rcode = dwarf_child(m_die, &child, &err);
    if (rcode == DW_DLV::NO_ENTRY) {
      return dw_die(m_dbg, nullptr);
    }
    else if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    else {
      return dw_die(m_dbg, child);
    }
  }
  
  dw_die dw_die::sibling() {
    check_not_null(m_die, "Contained DIE was null");
    Dwarf_Die sibling;
    Dwarf_Error err;
    int rcode = dwarf_siblingof_b(m_dbg, m_die, true, &sibling, &err);
    if (rcode == DW_DLV::NO_ENTRY) {
      return dw_die(m_dbg, nullptr);
    }
    else if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    else {
      return dw_die(m_dbg, sibling);
    }
  }
  
  // dw_attribute
  
  DW_FORM dw_attribute::form() {
    
    
    Dwarf_Error err;
    
    uint16_t form;
    int rcode = dwarf_whatform(m_attr, &form, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return static_cast<DW_FORM>(form);
  }
  
  DW_AT dw_attribute::name() {
    Dwarf_Error err;
    
    uint16_t form;
    int rcode = dwarf_whatattr(m_attr, &form, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    return static_cast<DW_AT>(form);
  }
  
  string dw_attribute::as_string() {
    Dwarf_Error err;
    
    char* cstr;
    int rcode = dwarf_formstring(m_attr, &cstr, &err);
    if (rcode == DW_DLV::ERROR) {
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
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    Dwarf_Die die;
    rcode = dwarf_offdie_b(m_dbg, offset, true, &die, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return dw_die(m_dbg, die);
  }
  
  uint64_t dw_attribute::as_unsigned_int() {
    Dwarf_Error err;
    
    uint64_t result;
    int rcode = dwarf_formudata(m_attr, &result, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return result;
  }
  
  int64_t dw_attribute::as_signed_int() {
    Dwarf_Error err;
    
    int64_t result;
    int rcode = dwarf_formsdata(m_attr, &result, &err);
    if (rcode == DW_DLV::ERROR) {
      throw dw_error(m_dbg, err);
    }
    
    return result;
  }
  
  // dw_attribute_list
  
  dw_attribute dw_attribute_list::operator[](size_t index) const {
    if (index >= m_size) {
      stringstream fmt;
      fmt << "Index " << index << " out of range for size " << m_size;
      throw std::out_of_range(fmt.str());
    }
    
    return dw_attribute(m_dbg, m_attrs[index], false);
  }
  
  dw_attribute_list::~dw_attribute_list() {
    for (size_t i = 0; i < m_size; i++) {
      dwarf_dealloc_attribute(m_attrs[i]);
    }
    dwarf_dealloc(m_dbg, (void*) m_attrs, DW_DLA_LIST);
  }
}