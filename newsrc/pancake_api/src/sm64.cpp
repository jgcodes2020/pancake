#include "pancake/dwarf/types.hpp"
#include "pancake/expr/parse.hpp"
#include <pancake/sm64.hpp>

#include <stdexcept>
#include <string>
#include <filesystem>

#include <unordered_map>
#include <pancake/dl/pdl.hpp>
#include <pancake/dwarf/types.hpp>
#include <pancake/expr/compile.hpp>

using std::string;
namespace fs = std::filesystem;

namespace {
  struct expr_info {
    void* ptr;
    bool is_complete;
    pancake::dwarf::base_type_info type;
  };
}

namespace pancake {
  struct sm64::impl {
    dl::library lib;
    dwarf::debug dbg;
    
    impl(fs::path path) :
      lib(path), dbg(path) {}
      
    void* get(const string& expr, pancake::dwarf::base_type_info type) {
      // check the pointer cache first
      static std::unordered_map<string, expr_info> cache;
      {
        decltype(cache)::iterator it;
        if ((it = cache.find(expr)) != cache.end()) {
          expr_info& info = it->second;
          if (info.type.encoding != dwarf::encoding::none && info.type != type) {
            throw std::invalid_argument("Type mismatch");
          }
          return info.ptr;
        }
      }
      
      expr::expr_eval eval = expr::compile(expr::parse(expr), dbg);
      
      uint8_t* ptr = &this->lib.get_symbol(eval.global);
    }
    
    
  };
}