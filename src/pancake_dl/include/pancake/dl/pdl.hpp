#ifndef _PANCAKE_DL_PDL_HPP_
#define _PANCAKE_DL_PDL_HPP_

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <utility>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux)
  #include <dlfcn.h>
#endif

namespace pancake::dl {
  namespace details {

    template <typename T>
    struct sym_cast {
      using return_type = T&;
      static return_type cast(void* ptr) { return *reinterpret_cast<T*>(ptr); }
    };
    
    template<>
    struct sym_cast<void> {
      using return_type = void*;
      static return_type cast(void* ptr) { return ptr; }
    };

    template <typename T>
    struct sym_cast<T[]> {
      using return_type = T*;
      static return_type cast(void* ptr) { return reinterpret_cast<T*>(ptr); }
    };
  }  // namespace details

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

    void* _impl_get_symbol(const std::string& name) const;

  public:
    library(const std::filesystem::path& path);

    ~library();

    /// Returns a native handle.
    handle native_handle() const;

    template <typename T = void>
    typename details::sym_cast<T>::return_type get_symbol(
      const std::string& name) const {
      return details::sym_cast<T>::cast(_impl_get_symbol(name));
    }

    section get_section(const std::string& name) const;
  };
}  // namespace pancake::dl
#endif