/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_UTILS_STRCVT_HPP_
#define _PANCAKE_UTILS_STRCVT_HPP_

#include <locale>
#include <sstream>
#include <string>
#include <string_view>

namespace pancake::strcvt {

  std::string to_char_string(std::wstring str);
  std::wstring to_utf16(std::string str);
}  // namespace pancake::strcvt
#endif