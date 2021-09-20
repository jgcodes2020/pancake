#include "pancake/dwarf/types.hpp"
#include "pancake/expr/parse.hpp"
#include "pancake/stx/overload.hpp"
#include <pancake/sm64.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>

#include <gsl/span>

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
      lib(path), dbg(path) {
      lib.get_symbol<void()>("sm64_init")();
    }
      
    void* get(const string& expr, pancake::dwarf::base_type_info type) {
      // check the pointer cache first
      static std::unordered_map<string, expr_info> cache;
      {
        decltype(cache)::iterator it;
        if ((it = cache.find(expr)) != cache.end()) {
          expr_info& info = it->second;
          if (type.encoding != dwarf::encoding::none && info.type != type) {
            throw std::invalid_argument("Type mismatch");
          }
          return info.ptr;
        }
      }
      
      auto data = expr::compile(expr::parse(expr), dbg);

      
      expr::expr_eval& eval = data.first;
      
      uint8_t* ptr = &this->lib.get_symbol<uint8_t>(eval.global);
      for (auto& step: eval.steps) {
        std::visit(stx::overload {
          [&](expr::expr_eval::offset step) mutable {
            ptr += step.off;
          },
          [&](expr::expr_eval::indirect step) mutable {
            ptr = *reinterpret_cast<uint8_t**>(ptr);
          }
        }, step);
      }
      
      cache[expr] = expr_info {
        ptr, true, data.second
      };
      
      if (type.encoding != dwarf::encoding::none && type != data.second) {
        throw std::invalid_argument("Type mismatch");
      }
      
      return ptr;
    }
    
    void advance() {
      lib.get_symbol<void()>("sm64_advance")();
    }
  };
  
  sm64::sm64(fs::path path) {
    p_impl = std::make_unique<impl>(path);
  }
  
  void* sm64::_impl_get(const string& expr, pancake::dwarf::base_type_info type) {
    return p_impl->get(expr, type);
  }
  
  sm64::savestate sm64::alloc_svst() const {
    return sm64::savestate(*this);
  }
  
  void sm64::advance() {
    p_impl->advance();
  }
  
  struct sm64::savestate::impl {
    std::array<gsl::span<char>, 2> regions;
    std::array<std::pair<std::unique_ptr<char[]>, size_t>, 2> buffers;
    
    const sm64& game;
    
    impl(const sm64& game_p) : game(game_p) {
      auto sect_data = game.p_impl->lib.get_section(".data");
      auto sect_bss = game.p_impl->lib.get_section(".bss");
      regions[0] = gsl::span<char> {
        reinterpret_cast<char*>(sect_data.ptr),
        sect_data.size
      };
      regions[1] = gsl::span<char> {
        reinterpret_cast<char*>(sect_bss.ptr),
        sect_bss.size
      };
      
      buffers[0].first = std::make_unique<char[]>(regions[0].size());
      buffers[0].second = regions[0].size();
      
      buffers[1].first = std::make_unique<char[]>(regions[1].size());
      buffers[1].second = regions[1].size();
    }
    
    void save() {
      std::copy(regions[0].begin(), regions[0].end(), buffers[0].first.get());
      std::copy(regions[1].begin(), regions[1].end(), buffers[1].first.get());
    }
    
    void load() const {
      std::copy(buffers[0].first.get(), buffers[0].first.get() + buffers[0].second, regions[0].begin());
      std::copy(buffers[1].first.get(), buffers[1].first.get() + buffers[1].second, regions[1].begin());
    }
  };
  
  sm64::savestate::savestate(const sm64& game) {
    p_impl = std::make_unique<impl>(game);
  }
  
  void sm64::savestate::save() {
    p_impl->save();
  }
  
  void sm64::savestate::load() const {
    p_impl->load();
  }
}