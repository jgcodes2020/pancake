#include <cstdint>
#include <functional>
#include <typeinfo>
#include <utility>
#include <pancake/dwarf/type_lookup.hpp>
#include <pancake/stx/pair_hash.hpp>


namespace pdwarf {
  const std::type_info& lookup_type(encoding encoding, size_t byte_size) {
    const static std::unordered_map<std::pair<pdwarf::encoding, size_t>, const std::type_info&> types {
      // Unsigned types
      {{encoding::unsigned_int, 1}, typeid(uint8_t)},
      {{encoding::unsigned_int, 2}, typeid(uint16_t)},
      {{encoding::unsigned_int, 4}, typeid(uint32_t)},
      {{encoding::unsigned_int, 8}, typeid(uint64_t)},
      
      // Signed types
      {{encoding::signed_int, 1}, typeid(int8_t)},
      {{encoding::signed_int, 2}, typeid(int16_t)},
      {{encoding::signed_int, 4}, typeid(int32_t)},
      {{encoding::signed_int, 8}, typeid(int64_t)},
      
      // Floating point
      // long double gets 12 AND 16, because alignment is wack
      {{encoding::floating_point, 4}, typeid(float)},
      {{encoding::floating_point, 8}, typeid(double)},
      {{encoding::floating_point, 12}, typeid(long double)},
      {{encoding::floating_point, 16}, typeid(long double)},
    };
    return types.at(std::make_pair(encoding, byte_size));
  }
}