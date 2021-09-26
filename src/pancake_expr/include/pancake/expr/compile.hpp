/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_EXPR_COMPILE_HPP_
#define _PANCAKE_EXPR_COMPILE_HPP_
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <iostream>

#include <pancake/expr/parse.hpp>
#include <pancake/dwarf/types.hpp>
#include <pancake/dwarf/type_info.hpp>
#include <pancake/stx/overload.hpp>

namespace pancake::expr {
  /**
   * @brief Represents a compiled expression, which can be quickly evaluated.
   */
  struct expr_eval {
    /**
     * @brief Instruction to offset by a specific value.
     */
    struct offset {
      intptr_t off;
    };
    /**
     * @brief Instruction to indirect the current pointer.
     */
    struct indirect {};
    
    using step = std::variant<offset, indirect>;
    using result_type = std::variant<dwarf::base_type_info, dwarf::die>;
    
    std::string start;
    std::vector<step> steps;
    dwarf::base_type_info result;
    
    expr_eval& operator+=(expr_eval&& eval) {
      std::copy(eval.steps.begin(), eval.steps.end(), std::back_inserter(steps));
      result = eval.result;
      return *this;
    }
  };
  
  
  
  inline std::ostream& operator<<(std::ostream& out, const expr_eval::step& s) {
    visit(stx::overload {
        [&](expr_eval::offset step) mutable -> void {
          out << " -> offset by " << step.off;
        },
        [&](expr_eval::indirect step) mutable -> void {
          out << " -> indirect";
        }
      }, s);
    return out;
  }
  
  inline std::ostream& operator<<(std::ostream& out, const expr_eval& e) {
    out << "get " << e.start;
    for (auto& i : e.steps) {
      visit(stx::overload {
        [&](expr_eval::offset step) mutable -> void {
          out << " -> offset by " << step.off;
        },
        [&](expr_eval::indirect step) mutable -> void {
          out << " -> indirect";
        }
      }, i);
    }
    return out;
  }
  
  /**
   * @brief Compiles an AST.
   * 
   * @param ast an AST to compile
   * @return const compiled_expr the offsets
   */
  expr_eval compile(const expr_ast& ast, pancake::dwarf::debug& dbg);
}
#endif