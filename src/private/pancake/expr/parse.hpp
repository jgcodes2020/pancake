/**
 * @file exparser.hpp
 * @author jgcodes2020
 * @brief Parser for "C-style accessor expressions"
 * @version 0.1
 * @date 2021-06-18
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_EXPR_PARSE_HPP_
#define _PANCAKE_EXPR_PARSE_HPP_

#include <cstdint>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <any>
#include <vector>
#include <variant>
/**
 * @brief Expression parser producing a list of "instructions."
 */
namespace pancake::expr {
  /**
   * @brief Parsed output. Not really a tree per se, but it may if I expand the syntax.
   */
  struct expr_ast {
    struct subscript {
      size_t index;
    };
    struct member {
      std::string name;
    };
    using step = std::variant<subscript, member>;
    std::string global;
    std::vector<step> steps;
  };
  
  inline std::ostream& operator<<(std::ostream& out, const expr_ast& code) {
    out << "get " << code.global;
    for (auto& i: code.steps) {
      out << " -> ";
      switch (i.index()) {
        case 0: {
          out << "subscript " << std::get<expr_ast::subscript>(i).index;
        } break;
        case 1: {
          out << "member " << std::get<expr_ast::member>(i).name;
        } break;
        case 2: {
          out << "dereference";
        }
        default: break;
      }
    }
    return out;
  }
  
  /**
   * @brief Preprocesses object fields onto an expression.
   * 
   * @param expr the expression to preprocess
   * @return The preprocessed string
   */
  std::string preprocess(std::string expr);
  
  /**
   * @brief Parses an expression into instructions.
   * 
   * @param expr the expression to parse 
   * @return An "access code" containing info on how to retrieve the desired value.
   */
  const expr_ast parse(std::string expr);
  
  
  class util {
  public:
    util() = delete;
    /**
     * @brief Parses an integer literal.
     * 
     * @param expr 
     * @return const size_t 
     */
    inline static size_t parse_int_literal(std::string expr) {
      if (expr.size() > 2 && (expr.rfind("0b", 0) || expr.rfind("0B", 0))) {
        return static_cast<size_t>(std::stoll(expr.substr(2)), 2);
      }
      else {
        return static_cast<size_t>(std::stoll(expr));
      }
    }
  };
}
#endif