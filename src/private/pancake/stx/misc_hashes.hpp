/***************************
The source code below is licensed under the BSD Zero-Clause License.
See the README.md in this directory for details.
***************************/

#ifndef _PANCAKE_STX_PAIR_HASH_HPP_
#define _PANCAKE_STX_PAIR_HASH_HPP_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#define PANCAKE_STX_ENUM_CLASS_HASH(T)               \
  template <>                                        \
  struct hash<T> {                                   \
    size_t operator()(T x) const {                   \
      using hash_t = std::underlying_type_t<T>;      \
      return hash<hash_t>()(static_cast<hash_t>(x)); \
    }                                                \
  };

namespace std {

  template <typename T, typename U>
  struct hash<pair<T, U>> {
    size_t operator()(pair<T, U> pair) const {
      // adapted from OpenJDK java.util.Arrays.hashCode(Object...)
      return 31 * hash<T>()(pair.first) + hash<U>()(pair.second);
    }
  };
}  // namespace std
#endif