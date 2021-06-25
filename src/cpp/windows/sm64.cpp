#include "pancake/sm64.hpp"

#include <string>
#include <locale>
#include <regex>

#include <windows.h>

namespace dwarf {
  #include <libdwarf.h>
}

#include "pancake/exparser.hpp"
#include "pancake/utils.hpp"
#include "pancake/dwarf.hpp"

using std::string;

namespace pancake {
  struct sm64::impl {
  private:
    
    
    
  public:
    HMODULE dll;
    dwarf::Dwarf_Debug dbg;
    
    impl(string path) : 
      dll(LoadLibraryW(strcvt::to_utf16(path).c_str())),
      dbg(dwarf::init(path)) {
    }
      
    any read(string expr) {
      // Parse expression
      exparser::ast ast = exparser::parse(expr);
      
      
    }
  };
}