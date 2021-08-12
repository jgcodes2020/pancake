#ifndef _PANCAKE_DWARF_TYPELOOKUP_HPP_
#define _PANCAKE_DWARF_TYPELOOKUP_HPP_
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <pancake/dwarf/enums.hpp>
namespace pdwarf {
  const std::type_info& lookup_type(encoding encoding, size_t byte_size);
}
#endif