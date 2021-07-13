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
__forceinline uint32_t _intrin_ffs64(uint64_t i) {
  unsigned long x;
  if (_BitScanReverse64(&x, i))
    return x;
  else
   return 64;
}
#elif defined(__GNUC__) || defined(__MINGW32__)
// Highest set bit.
__attribute__((always_inline)) uint32_t _intrin_ffs(uint64_t i) {
  return __builtin_ffsll(i) - 1;
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
    
    const uint32_t max_shift = _intrin_ffs64(x);
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