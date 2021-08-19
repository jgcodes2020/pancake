/***************************
The source code below is licensed under the BSD Zero-Clause License.
See the README.md in this directory for details.

Many people will probably have written this in some other form,
so I have no reason to go after anyone.
***************************/

#ifndef _PANCAKE_STX_OVERLOAD_HPP
#define _PANCAKE_STX_OVERLOAD_HPP

namespace stx {
  /**
  * @brief Object that overloads a set of functors (lambdas are functors)
  */
  template<typename ... _Ts>
  struct overload : public _Ts... {
    using _Ts::operator()...;
  };
  template<typename ... _Ts>
  overload(_Ts...) -> overload<_Ts...>;
}
#endif