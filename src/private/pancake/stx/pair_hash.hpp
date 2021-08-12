#ifndef _PANCAKE_STX_PAIR_HASH_HPP_
#define _PANCAKE_STX_PAIR_HASH_HPP_

#include <utility>
#include <functional>

namespace std {
  template<typename T, typename U>
  struct hash<pair<T, U>> {
    size_t operator()(pair<T, U> pair) {
      // adapted from OpenJDK java.util.Arrays.hashCode(Object...)
      return 31 * hash<T>(pair.first) + hash<U>(pair.second);
    }
  };
}
#endif