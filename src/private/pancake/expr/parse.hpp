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
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <any>
#include <vector>
#include <map>
#include <regex>
#include <variant>

#include <pancake/overload.hpp>

inline std::regex operator"" _re(const char* str, size_t size) { return std::regex(str); }
/**
 * @brief Expression parser producing a list of "instructions."
 */
namespace pancake::expr {
  namespace lexer {
    struct token {
    private:
    public:
      std::string text;
      enum class type : uint16_t {
        identifier,
        number,
        subscript_begin,
        subscript_end,
        member_op
      };
      type t_type;
      ptrdiff_t index;
    };
    static const std::map<token::type, std::regex>& type_regexes() {
      static std::map<token::type, std::regex> instance {
        {token::type::identifier, R"/(^([A-Za-z_]\w*))/"_re},
        {token::type::number,
         R"/(^(?:((?:[1-9]\d+)|\d)|(?:0[0-7]+)|(?:0x[\dA-Fa-f]+)|(?:0b[01]+)))/"_re},
        {token::type::subscript_begin, R"/(^\[)/"_re},
        {token::type::subscript_end, R"/(^\])/"_re},
        {token::type::member_op, R"/(^(?:\.|(?:->)))/"_re},
      };
      return instance;
    }
  }  // namespace lexer
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
  
  inline std::ostream& operator<<(std::ostream& out, const expr_ast::step& step) {
    visit(overload {
      [&](expr_ast::subscript step) mutable -> void {
        out << "subscript " << step.index;
      },
      [&](expr_ast::member step) mutable -> void {
        out << "member " << step.name;
      },
    }, step);
    return out;
  }
  
  inline std::ostream& operator<<(std::ostream& out, const expr_ast& code) {
    out << "get " << code.global;
    for (auto& i: code.steps) {
      out << " -> ";
      visit(overload {
        [&](expr_ast::subscript step) mutable -> void {
          out << "subscript " << step.index;
        },
        [&](expr_ast::member step) mutable -> void {
          out << "member " << step.name;
        },
      }, i);
    }
    return out;
  }
  
  /**
   * @brief Parses an expression into instructions.
   * 
   * @param expr the expression to parse 
   * @return An "access code" containing info on how to retrieve the desired value.
   */
  expr_ast parse(std::string expr);
  
  
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