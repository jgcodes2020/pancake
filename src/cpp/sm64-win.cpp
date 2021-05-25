#include <cstdint>

#include <string>
#include <array>
#include <stdexcept>

#include <windows.h>

#include "LIEF/LIEF.hpp"
#include "pancake/sm64.hpp"

using std::string, std::array;
//A function that takes nothing and returns nothing.
//Equivalent to java.lang.Runnable, hence the name.
typedef void (*runnable_fn)();
// These look much cleaner with hex numbers
const array<array<pancake::sm64::mem_region, 2>, 2> save_sections {
  // US Version
  array<pancake::sm64::mem_region, 2> {
    pancake::sm64::mem_region {0x13E000, 0x2B4860},
    pancake::sm64::mem_region {0xD65000, 0x4ABA80}
  },
  // JP Version
  array<pancake::sm64::mem_region, 2> {
    pancake::sm64::mem_region {0x13C000, 0x24B6E0},
    pancake::sm64::mem_region {0xCF7000, 0x4ABB60}
  }
};

pancake::sm64::sm64(string path, pancake::sm64::version version) {
  _version = version;
  _lib = LoadLibraryA(path.c_str());
  if (_lib == nullptr) {
    auto error = pancake::shared_lib_error("Could not load SM64");
    error.error_code = GetLastError();
    throw error;
  }
  //call sm64_init
  ((runnable_fn) GetProcAddress(static_cast<HMODULE>(_lib), "sm64_init"))();
  //throws range error if we get invalid version
  array<pancake::sm64::mem_region, 2> data = save_sections.at(static_cast<size_t>(version));
  for (int i = 0; i < 2; i++) 
    _regions[i] = data[i];
}
pancake::sm64::~sm64() {
  FreeLibrary(static_cast<HMODULE>(_lib));
}

void* pancake::sm64::_impl_locate(string symbol) {
  FARPROC result = GetProcAddress(static_cast<HMODULE>(_lib), symbol.c_str());
  if (result == NULL) {
    auto error = pancake::shared_lib_error("Could not load symbol " + symbol);
    error.error_code = GetLastError();
    throw error;
  }
  else
    return reinterpret_cast<void*>(result);
}

void pancake::sm64::advance() {
  //call sm64_update
  ((runnable_fn) GetProcAddress(static_cast<HMODULE>(_lib), "sm64_update"))();
}