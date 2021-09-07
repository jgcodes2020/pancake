#ifndef _PANCAKE_DWARF_TYPELOOKUP_HPP_
#define _PANCAKE_DWARF_TYPELOOKUP_HPP_
#include <string>
#include <typeinfo>
/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#include <pancake/stx/misc_hashes.hpp>
#include <utility>
#include <pancake/dwarf/enums.hpp>
namespace std {
  PANCAKE_STX_ENUM_CLASS_HASH(pdwarf::encoding)
}
namespace pdwarf {
  const std::type_info& lookup_type(encoding encoding, size_t byte_size);
}
#endif