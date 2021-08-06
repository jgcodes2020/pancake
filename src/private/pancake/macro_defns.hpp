/**
 * @file macro_defns.hpp
 * @author your name (you@domain.com)
 * @brief sm64_macro_defns parsed singleton.
 * @version 0.1
 * @date 2021-06-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _PANCAKE_MACRO_DEFNS_HPP
#define _PANCAKE_MACRO_DEFNS_HPP

#include <string>
#include <nlohmann/json.hpp>
#include <pancake_rsrc_sm64_macro_defns.json.hpp>

namespace pancake {
  class sm64_macro_defns {
  private:
    nlohmann::json json;
    sm64_macro_defns() {
      json = nlohmann::json::parse(
        resources::sm64_macro_defns__json
      );
    }
  public:
    [[nodiscard]] static nlohmann::json get() {
      static sm64_macro_defns instance;
      return instance.json;
    }
  };
}
#endif