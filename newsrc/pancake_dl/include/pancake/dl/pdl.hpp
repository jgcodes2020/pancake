#ifndef _PANCAKE_DL_PDL_HPP_
#define _PANCAKE_DL_PDL_HPP_

#include <filesystem>
#include <memory>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux)
#include <dlfcn.h>
#endif

namespace pancake::dl {
  
  #if defined(_WIN32)
  using handle = HMODULE;
  #elif defined(__linux__)
  using handle = void*;
  #endif
  
  class dl_error : std::logic_error {
    using std::logic_error::logic_error;
  };
  
  struct section {
    void* ptr;
    size_t size;
  };
  
  class library {
    struct impl;
    std::unique_ptr<impl> p_impl;
    
    void* _impl_get_symbol(std::string name);
  public:
    library(std::filesystem::path path);
    
    ~library() = default;
    
    /// Returns a native handle.
    handle native_handle();
    
    template<typename T>
    T& get_symbol(const std::string& name) {
      return *reinterpret_cast<T*>(_impl_get_symbol(name));
    }
    
    section get_section(const std::string& name);
  };
}
#endif