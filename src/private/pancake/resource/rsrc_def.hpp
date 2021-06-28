/**
 * @file resource_struct.hpp
 * @author jgcodes2020
 * @brief typedefs arrays as resources.
 * @version 0.1
 * @date 2021-06-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _PANCAKE_RSRC_DEF_HPP_
#define _PANCAKE_RSRC_DEF_HPP_
#include <sstream>
#include <stdint.h>
#include <iostream>
#include <array>
#include <streambuf>
 
namespace pancake::resources {
  template<size_t size>
  using resource = const std::array<char, size>;
 
  class rsrc_buf : std::streambuf {
  public:
    template<size_t size>
    rsrc_buf(resource<size> rsrc) {
      setg(&rsrc.front(), &rsrc.front(), &rsrc.back());
    }
  };
 
  template<size_t size>
  std::istream rsrc_as_stream(resource<size> rsrc) {
    return std::istream(rsrc_buf(rsrc));
  }
}
#endif