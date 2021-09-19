#ifndef _PANCAKE_MACRO_DEFNS_HPP_
#define _PANCAKE_MACRO_DEFNS_HPP_
#include <nlohmann/json.hpp>
#include <pancake_rsrc_sm64_macro_defns.json.h>
namespace pancake {
  class sm64_macro_defns {
  private:
    nlohmann::json json;
    sm64_macro_defns() {
      json = nlohmann::json::parse(_pancake_rsrc_sm64_macro_defns_json_begin, _pancake_rsrc_sm64_macro_defns_json_end - 1);  
    }
      
  public:
    static nlohmann::json get() {
      static sm64_macro_defns instance;
      return instance.json;
    }
  };
}
#endif