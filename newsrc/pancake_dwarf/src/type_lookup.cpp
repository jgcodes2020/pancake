/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <pancake/dwarf/enums.hpp>
#include <pancake/dwarf/type_lookup.hpp>
#include <pancake/stx/misc_hashes.hpp>

namespace pdwarf {
  const std::type_info& lookup_type(encoding encoding, size_t byte_size) {
    const static std::unordered_map<std::pair<enum encoding, size_t>, const std::type_info&> types ({
      // Unsigned types
      {{encoding::unsigned_char, 1}, typeid(uint8_t)},
      {{encoding::unsigned_int, 2}, typeid(uint16_t)},
      {{encoding::unsigned_int, 4}, typeid(uint32_t)},
      {{encoding::unsigned_int, 8}, typeid(uint64_t)},
      
      // Signed types
      {{encoding::signed_char, 1}, typeid(int8_t)},
      {{encoding::signed_int, 2}, typeid(int16_t)},
      {{encoding::signed_int, 4}, typeid(int32_t)},
      {{encoding::signed_int, 8}, typeid(int64_t)},
      
      // Floating point
      // long double gets 12 AND 16, because alignment is wack
      {{encoding::floating_point, 4}, typeid(float)},
      {{encoding::floating_point, 8}, typeid(double)},
      {{encoding::floating_point, 12}, typeid(long double)},
      {{encoding::floating_point, 16}, typeid(long double)},
    });
    std::cerr << "Looking up enc " << encoding << ", bytes " << byte_size;
    return types.at(std::pair<enum encoding, size_t>(encoding, byte_size));
  }
}