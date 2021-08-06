#include <system_error>
#include <pancake/dwarf/error.hpp>

using std::error_code, std::error_condition;

namespace pdwarf {
  const dwarf_category_impl& dwarf_category() {
    static dwarf_category_impl instance;
    return instance;
  }
  
  error_code make_error_code(dwarf_errc x) {
    return std::error_code(static_cast<int>(x), dwarf_category());
  }
  error_condition make_error_condition(dwarf_errc x) {
    return std::error_condition(static_cast<int>(x), dwarf_category());
  }
}