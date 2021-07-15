/**
 * @file compile.hpp
 * @author your name (you@domain.com)
 * @brief Compiler for "C-style accessor expression" ASTs
 * @version 0.1
 * @date 2021-06-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _PANCAKE_EXPR_COMPILE_HPP_
#define _PANCAKE_EXPR_COMPILE_HPP_
#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <iostream>

#include <pancake/dwarf.hpp>
#include <pancake/expr/parse.hpp>
#include <pancake/overload.hpp>

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
  
  inline std::ostream& operator<<(std::ostream& out, const expr_eval& e) {
    out << "get " << e.global;
    for (auto& i : e.steps) {
      visit(overload {
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
  const expr_eval compile(const expr_ast& ast, dwarf::dw_debug& dbg);
}
#endif