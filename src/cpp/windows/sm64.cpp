#include "pancake/sm64.hpp"

#include <string>
#include <locale>
#include <regex>

#include <windows.h>

#include "pancake/utils.hpp"
#include "pancake/dwarf.hpp"

using std::wstring, std::regex;
using std::regex_search, std::smatch;

namespace pancake {
  struct sm64::impl {
  private:
    inline static const regex RE_WORD      = regex("^(\\w+)\\b");
    inline static const regex RE_SUBSCRIPT = regex("^\\[(\\d+)\\]");
    inline static const regex RE_MEMBER    = regex("^(\\.|->)");
    
    enum class operation {
      SUBSCRIPT, MEMBER
    };
  public:
    HMODULE dll;
    dwarf::Dwarf_Debug dbg;
    
    impl(wstring path) :
      dll(LoadLibraryW(path.c_str())),
      dbg(dwarf::init(pancake::strcvt::to_utf8(path))) {}
      
    any read(string expr) {
    }
  };
}