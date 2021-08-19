/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_SHARED_LIB_HPP_
#define _PANCAKE_SHARED_LIB_HPP_

#include <string>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
  #include <dlfcn.h>
#endif

namespace pshared {
  #if defined(_WIN32)
  using handle_t = HMODULE;
  
  handle_t load_shared(std::string path);
  
  template<typename T>
  T get_symbol(handle_t dll, std::string symbol);
  
  void close_shared(handle_t dll);
  
  // Path is assumed to be UTF-8
  inline handle_t load_shared(std::string path) {
    
  }
  #elif defined(__linux__) || defined(__APPLE__)
  using handle_t = void*;
  #endif
}
#endif