/**
 * @file sm64-win.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#if defined(_WIN32)
#include <cstdint>

#include <string>
#include <array>
#include <stdexcept>

#include <algorithm>

#include <windows.h>

#include "libdwarf.h"

#include "pancake/sm64.hpp"

using std::string;
using namespace pancake;
//A function that takes nothing and returns nothing.
//Equivalent to java.lang.Runnable, hence the name.
typedef void (*runnable_fn)();

void nop() {}

struct sm64::impl {
  HMODULE m_module;

  impl(string path) {
    
  }

  void advance() {
    nop();
  }
};

sm64::sm64(string path) :
p_impl(new impl(path)) {}
#endif