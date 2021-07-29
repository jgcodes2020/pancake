#ifndef _PANCAKETEST_MATHUTIL_HPP_
#define _PANCAKETEST_MATHUTIL_HPP_

#include <cstdint>
#include <string>
#include <array>
#include <sstream>

using namespace std;

#ifdef _MSC_VER
#include <intrin.h>
// Highest set bit.
__forceinline inline uint32_t bsr(uint64_t i) {
  unsigned long x;
  if (_BitScanReverse64(&x, i))
    return x;
  else
   return 64;
}
#elif defined(__GNUC__)
__attribute__((always_inline)) inline uint32_t bsr(uint64_t i) {
  if (i == 0) return 64;
  return 63 - __builtin_clzll(i);
}
#else
// https://stackoverflow.com/questions/52657851/how-to-bit-scan-forward-and-reverse-a-uint128-t-128bit
inline uint32_t bsr(uint64_t i) {
  if (i == 0) return 64;
  // round up to power of 2 - 1
  i |= i >> 1;
  i |= i >> 2;
  i |= i >> 4;
  i |= i >> 8;
  i |= i >> 16;
  i |= i >> 32;
  // abuse special number and lookup
  const constexpr uint64_t magic = 0x03F79D71B4CB0A89ULL;
  static const std::uint32_t table[] = {
    0,  47, 1,  56, 48, 27, 2,  60, 57, 49, 41, 37, 28, 16, 3,  61,
    54, 58, 35, 52, 50, 42, 21, 44, 38, 32, 29, 23, 17, 11, 4,  62,
    46, 55, 26, 59, 40, 36, 15, 53, 34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30, 9,  24, 13, 18, 8,  12, 7,  6,  5,  63
  };
  return table[(i * magic) >> 58];
}
#endif

inline uint16_t fast_ilog10(uint64_t n) {
  uint64_t p10 = 1, result = 0, tmp;
  
  tmp = p10 * 1'0000'0000'0000'0000; // 10^16
  if (tmp < n) { p10 = tmp; result += 16; }
  tmp = p10 * 1'0000'0000; // 10^8
  if (tmp < n) { p10 = tmp; result += 8;  }
  tmp = p10 * 1'0000; //10^4
  if (tmp < n) { p10 = tmp; result += 4;  }
  tmp = p10 * 1'00; //10^2
  if (tmp < n) { p10 = tmp; result += 2;  }
  tmp = p10 * 1'0; //10^1
  if (tmp < n) { p10 = tmp; result += 1;  }
  return result;
}

inline string operator*(string str, uint32_t x) {
  if (x == 0)
    return "";
  else if (x == 1)
    return str;
  else {
    array<string, 32> times_p2;
    times_p2[0] = str;
    
    const uint32_t max_shift = bsr(x);
    for (uint32_t i = 1; i <= max_shift; i++) {
      times_p2[i] = times_p2[i - 1] + times_p2[i - 1];
    }
    
    stringstream out;
    for (uint32_t i = 0; i <= max_shift; i++) {
      if (x & (1 << i)) {
        out << times_p2[i];
      }
    }
    return out.str();
  }
}
// compact syntax for the string constructor
inline string operator "" _s(const char* str, size_t) {
  return string(str);
}

#endif