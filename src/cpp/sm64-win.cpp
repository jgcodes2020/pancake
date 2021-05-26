#include "LIEF/PE/Binary.hpp"
#include "LIEF/PE/Section.hpp"
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#if defined(_WIN32)
#include <cstdint>

#include <string>
#include <array>
#include <stdexcept>

#include <algorithm>

#include <windows.h>

#include "LIEF/LIEF.hpp"
#include "libdwarf.h"

#include "pancake/sm64.hpp"

using std::string;
using namespace pancake;
using namespace LIEF;
//A function that takes nothing and returns nothing.
//Equivalent to java.lang.Runnable, hence the name.
typedef void (*runnable_fn)();

void nop() {}

struct sm64::impl {
  HMODULE m_module;
  PE::Binary m_pe;

  impl(string path) :
  m_pe(path, PE::PE_TYPE::PE32_PLUS) {
    PE::Section debug = m_pe.get_section(".debug_info");

  }

  void advance() {
    nop();
  }
};

sm64::sm64(string path) :
p_impl(new impl(path)) {}
#endif