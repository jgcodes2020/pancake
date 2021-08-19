/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

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