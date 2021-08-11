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