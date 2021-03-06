#include <pancake/dl/pdl.hpp>
#include <LIEF/ELF.hpp>

#include <dlfcn.h>
#include <elf.h>
#include <link.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>


namespace fs = std::filesystem;
namespace ELF = LIEF::ELF;

namespace pancake::dl {
  struct library::impl {
    const handle hnd;
    std::unique_ptr<ELF::Binary> bin;
    
    impl(const fs::path& path) : 
      hnd([path]() {
        handle hnd = dlopen(path.c_str(), RTLD_NOW);
        if (hnd == nullptr) {
          std::string error = dlerror();
          throw dl_error(error);
        }
        return hnd;
      }()),
      bin(ELF::Parser::parse(path)) {}
    
    ~impl() {
      dlclose(hnd);
    }
    
    void* get_symbol(const std::string& name) const {
      void* sym = dlsym(hnd, name.c_str());
      if (sym == nullptr) {
        char* error = dlerror();
        std::cerr << error << "\n";
        throw dl_error(error);
      }
      
      return sym;
    }
    
    section get_section(const std::string& name) const {
      link_map* map;
      int rc = dlinfo(hnd, RTLD_DI_LINKMAP, &map);
      if (rc == -1) {
        throw dl_error(dlerror());
      }
      
      auto sect = bin->get_section(name);
      
      return section {
        reinterpret_cast<void*>(map->l_addr + sect.virtual_address()),
        sect.size()
      };
    }
  };
  
  library::library(const fs::path& path) :
    p_impl(new impl(path)) {}
  
  library::~library() = default;
  
  void* library::_impl_get_symbol(const std::string& str) const {
    return p_impl->get_symbol(str);
  }
  
  handle library::native_handle() const {
    return p_impl->hnd;
  }
  
  section library::get_section(const std::string& name) const {
    return p_impl->get_section(name);
  }
}