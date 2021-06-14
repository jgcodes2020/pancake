/**
 * @file experiment.cpp
 * @author jgcodes2020
 * @brief A test program that I am using to experiment.
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdexcept>
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
  
namespace dwarf {
  #include <libdwarf.h>
  #include <dwarf.h>
}

using namespace std;

int main(int argc, char* argv[]) {
  int dw_rcode;
  
  dwarf::Dwarf_Error error;
  dwarf::Dwarf_Debug instance;
  
  dw_rcode = dwarf::dwarf_init_path(
    argv[1], NULL, 0,                // File args
    DW_DLC_READ, DW_GROUPNUMBER_ANY, // File read args
    NULL, NULL, &instance,           // DWARF params
    NULL, 0, NULL,                   // Reserved
    &error                           // Error
  );
  if (dw_rcode != DW_DLV_OK) {
    string errmsg = dwarf::dwarf_errmsg(error);
    dwarf::dwarf_dealloc_error(instance, error);
    throw runtime_error(errmsg);
  }
  
  cout << "DWARF is initialized!" << endl;
  
  {
    cout << "List of translation units:" << endl;
    dwarf::Dwarf_Unsigned cu_header_len, abbrev_off, next_cu;
    dwarf::Dwarf_Half version_stamp, address_size;
    
    for (size_t cu_count = 0; ; cu_count++) {
      dwarf::Dwarf_Die cu_die = NULL;
      // Try to grab the next compile unit header
      dw_rcode = dwarf::dwarf_next_cu_header(
        instance, 
        &cu_header_len, &version_stamp, &abbrev_off,
        &address_size, &next_cu, &error
      );
      if (dw_rcode == DW_DLV_ERROR) {
        string errmsg = dwarf::dwarf_errmsg(error);
        dwarf::dwarf_dealloc_error(instance, error);
        throw runtime_error(errmsg);
      }
      else if (dw_rcode == DW_DLV_NO_ENTRY) {
        break;
      }
      // Use a null DIE to get the CU die
      dw_rcode = dwarf::dwarf_siblingof(instance, NULL, &cu_die, &error);
      
      if (dw_rcode == DW_DLV_ERROR) {
        string errmsg = dwarf::dwarf_errmsg(error);
        dwarf::dwarf_dealloc_error(instance, error);
        throw runtime_error(errmsg);
      }
      else if (dw_rcode == DW_DLV_NO_ENTRY) {
        cerr << "The debug data is corrupted. Go rebuild your executable." << endl;
        throw runtime_error("Data is corrupt");
      }
      // get name attribute
      dwarf::Dwarf_Attribute name_attr;
      dwarf::dwarf_attr(cu_die, DW_AT_name, &name_attr, &error);
      if (dw_rcode != DW_DLV_OK) {
        string errmsg = dwarf::dwarf_errmsg(error);
        dwarf::dwarf_dealloc_error(instance, error);
        throw runtime_error(errmsg);
      }
      
      char* name_cstr = NULL;
      dwarf::dwarf_formstring(name_attr, &name_cstr, &error);
      if (dw_rcode != DW_DLV_OK) {
        string errmsg = dwarf::dwarf_errmsg(error);
        dwarf::dwarf_dealloc_error(instance, error);
        throw runtime_error(errmsg);
      }
      cout << name_cstr << endl;
      dwarf::dwarf_dealloc_attribute(name_attr);
      dwarf::dwarf_dealloc_die(cu_die);
    }
  }
  
  dw_rcode = dwarf::dwarf_finish(instance, &error);
  if (dw_rcode != DW_DLV_OK) {
    string errmsg = dwarf::dwarf_errmsg(error);
    dwarf::dwarf_dealloc_error(instance, error);
    throw runtime_error(errmsg);
  }
  
  cout << "DWARF is closed!" << endl;
  
  return 0;
}