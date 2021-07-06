/*
This is very simple code, and has probably already been made before.
 */

#ifndef _PANCAKE_OVERLOAD_HPP
#define _PANCAKE_OVERLOAD_HPP

/**
 * @brief Object that overloads a list of lambdas.
 */
template<typename ... Ts>
struct overload : public Ts... {
  using Ts::operator()...;
};
template<typename ... Ts>
overload(Ts...) -> overload<Ts...>;

#endif