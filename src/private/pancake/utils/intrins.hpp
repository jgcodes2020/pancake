/**
 * @file intrins.hpp
 * @author jgcodes2020
 * @brief Compiler-independent intrinsics.
 * @version 0.1
 * @date 2021-07-17
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_UTILS_INTRINS_HPP_
#define _PANCAKE_UTILS_INTRINS_HPP_

#include <cstdint>

#if defined(_MSC_VER)
  #include <intrin.h>
  #define _pancake_force_inline inline __forceinline
#elif defined(__GNUC__)
  #define _pancake_force_inline inline __attribute__((__alwaysinline))
#endif

namespace intrinsics {
  /**
   * @brief Returns the number of set bits in an unsigned integer.
   * 
   * @param x the value to check
   * @return the number of set bits
   */
  template<typename T>
  _pancake_force_inline uint16_t popcount(T x);
  
  template<>
  _pancake_force_inline uint16_t popcount(uint16_t x) {
    #if defined(_MSC_VER)
    return __popcnt16(x);
    #elif defined(__GNUC__)
    return __builtin_popcount(x);
    #else
    x = x - ((x >> 1) & 0x5555);
    x = (x & 0x3333) + ((x >> 2) & 0x3333);
    x = (x + (x >> 4)) & 0x0F0F;
    return (x & 0x00FF) + (x >> 8);
    #endif
  }
  
  template<>
  _pancake_force_inline uint16_t popcount(uint32_t x) {
    #if defined(_MSC_VER)
    return __popcnt(x);
    #elif defined(__GNUC__)
    return __builtin_popcountl(x);
    #else
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;
    #endif
  }
  
  template<>
  _pancake_force_inline uint16_t popcount(uint64_t x) {
    #if defined(_MSC_VER)
    return __popcnt64(x);
    #elif defined(__GNUC__)
    return __builtin_popcountll(x);
    #else
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F;
    return (x * 0x0101010101010101) >> 56;
    #endif
  }
}

#endif