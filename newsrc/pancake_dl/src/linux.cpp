#include <pancake/dl/pdl.hpp>
#include <LIEF/ELF.hpp>

#include <dlfcn.h>
#include <elf.h>
#include <link.h>

#include <filesystem>
#include <memory>


namespace fs = std::filesystem;
namespace ELF = LIEF::ELF;

namespace pancake.dl {
  struct library::impl {
    handle hnd;
    std::unique_ptr<ELF::Binary> bin;
    
    impl(fs::path path) : 
      hnd([path]() {
        handle hnd = dlopen(path.c_str(), RTLD_LAZY);
        if (hnd == nullptr) {
          throw dl_error(dlerror());
        }
        return hnd;
      }()),
      bin(ELF::Parser::parse(path)) {}
    
    ~impl() {
      dlclose(hnd);
    }
    
    void* get_symbol(const std::string& name) {
      void* sym = dlsym(hnd, name.c_str());
      if (!sym) {
        throw dl_error(dlerror());
      }
      
      return sym;
    }
    
    section get_section(const std::string& name) {
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
}