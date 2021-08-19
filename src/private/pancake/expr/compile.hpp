/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_EXPR_COMPILE_HPP_
#define _PANCAKE_EXPR_COMPILE_HPP_
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <iostream>

#include <libdwarf/libdwarf.h>
#include <pancake/expr/parse.hpp>
#include <pancake/dwarf/functions.hpp>
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
    std::string global;
    std::vector<step> steps;
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
    out << "get " << e.global;
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
  const std::pair<expr_eval, const std::type_info&> compile(const expr_ast& ast, std::shared_ptr<Dwarf_Debug_s>& dbg);
}
#endif