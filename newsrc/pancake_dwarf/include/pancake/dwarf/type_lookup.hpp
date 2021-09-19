/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DWARF_TYPELOOKUP_HPP_
#define _PANCAKE_DWARF_TYPELOOKUP_HPP_
#include <cmath>
#include <limits>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <pancake/dwarf/enums.hpp>
#include <pancake/stx/misc_hashes.hpp>
namespace std {
  PANCAKE_STX_ENUM_CLASS_HASH(pancake::dwarf::encoding)
}
namespace pancake::dwarf {

  struct dwarf_type_info {
    encoding encoding;
    size_t size;
  };

  template <typename _T>
  constexpr dwarf_type_info get_type_info() {
    static_assert(
      std::is_arithmetic_v<_T> && !std::is_const_v<_T> &&
        !std::is_volatile_v<_T>,
      "Type must be arithmetic with no CV qualifiers");

    dwarf_type_info result;
    if constexpr (std::is_same_v<_T, char>) {
      if constexpr (std::numeric_limits<_T>::is_signed) {
        result.encoding = encoding::signed_char;
      }
      else {
        result.encoding = encoding::unsigned_char;
      }
    }
    else if constexpr (std::is_same_v<_T, unsigned char>) {
      result.encoding = encoding::unsigned_char;
    }
    else if constexpr (std::is_same_v<_T, signed char>) {
      result.encoding = encoding::signed_char;
    }
    else if constexpr (std::is_integral_v<_T>){
      if constexpr (std::is_signed_v<_T>) {
        result.encoding = encoding::signed_int;
      }
      else {
        result.encoding = encoding::unsigned_int;
      }
    }
    else if constexpr (std::is_floating_point_v<_T>) {
      result.encoding = encoding::floating_point;
    }
    result.size = sizeof(_T);
    
    return result;
  }
}  // namespace pancake::dwarf
#endif