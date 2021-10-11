/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DWARF_TYPELOOKUP_HPP_
#define _PANCAKE_DWARF_TYPELOOKUP_HPP_
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <pancake/dwarf/enums.hpp>

namespace pancake::dwarf {
  struct any_type_info;

  struct base_type_info {
    dwarf::encoding encoding;
    size_t size;
    
    bool operator==(const base_type_info& other) const {
      return encoding == other.encoding && size == other.size;
    }
    
    bool operator!=(const base_type_info& other) const {
      return !(*this == other);
    }
  };

  template <typename _T>
  constexpr base_type_info get_type_info() {
    static_assert(
      std::is_arithmetic_v<_T> && !std::is_const_v<_T> &&
        !std::is_volatile_v<_T>,
      "Type must be arithmetic with no CV qualifiers");

    base_type_info result;
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