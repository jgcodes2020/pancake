/**
 * @file utils.hpp
 * @author your name (you@domain.com)
 * @brief A bunch of useful functions.
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_STRCVT_HPP_
#define _PANCAKE_STRCVT_HPP_

#include <string>
#include <locale>
#include <codecvt>

namespace pancake {
  /**
   * @brief String conversion stuff.
   */
  namespace strcvt {
    /**
     * @brief The UTF-8 locale.
     */
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    
    inline std::string to_utf8(std::wstring str) {
      return converter.to_bytes(str);
    }
    inline std::wstring to_utf16(std::string str) {
      return converter.from_bytes(str);
    }
  }
  
}
#endif