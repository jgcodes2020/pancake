/**
 * @file utils.hpp
 * @author your name (you@domain.com)
 * @brief A bunch of useful functions.
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_UTILS_HPP_
#define _PANCAKE_UTILS_HPP_

#include <string>
#include <locale>

namespace pancake {
  /**
   * @brief String conversion stuff.
   */
  namespace strcvt {
    using std::string, std::wstring;
    /**
     * @brief The UTF-8 locale.
     */
    const std::locale UTF8 = std::locale("en_US.utf8");
    
    string to_utf8(wstring str) {
      using mb2wc_t = std::codecvt<wchar_t, char, std::mbstate_t>;
      auto& cvt = std::use_facet<mb2wc_t>(UTF8);
      
      std::mbstate_t mb = std::mbstate_t{};
      string res(str.size() * cvt.max_length(), '\0');
      const wchar_t* from_n; char* to_n;
      cvt.out(mb,
        &str[0], &str[str.size()], from_n,
        &res[0], &res[res.size()], to_n
      );
      
      res.resize(to_n - &res[0]);
      return res;
    }
  }
  
}
#endif