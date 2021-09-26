/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

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
#include <list>
#include <regex>
#include <variant>

#include <pancake/stx/overload.hpp>

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
      enum class type_t : uint16_t {
        identifier,
        number,
        subscript_begin,
        subscript_end,
        dot,
        arrow,
      };
      type_t type;
      ptrdiff_t index;
    };
    static const std::list<std::pair<token::type_t, std::regex>>& type_regexes() {
      static std::list<std::pair<token::type_t, std::regex>> instance {
        {token::type_t::identifier, R"/(^([A-Za-z_]\w*)\b)/"_re},
        {token::type_t::number,
         R"/(^(?:((?:[1-9]\d+)|\d)|(?:0[0-7]+)|(?:0x[\dA-Fa-f]+)|(?:0b[01]+)))/"_re},
        {token::type_t::subscript_begin, R"/(^\[)/"_re},
        {token::type_t::subscript_end, R"/(^\])/"_re},
        {token::type_t::dot, R"/(^\.)/"_re},
        {token::type_t::arrow, R"/(^->)/"_re}
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
    struct dot {
      std::string name;
    };
    struct arrow {
      std::string name;
    };
    using step = std::variant<subscript, dot, arrow>;
    std::string global;
    std::vector<step> steps;
  };
  
  inline std::ostream& operator<<(std::ostream& out, const expr_ast::step& step) {
    visit(stx::overload {
      [&](expr_ast::subscript step) mutable -> void {
        out << "subscript " << step.index;
      },
      [&](expr_ast::dot step) mutable -> void {
        out << "member " << step.name;
      },
      [&](expr_ast::arrow step) mutable -> void {
        out << "deref+member " << step.name;
      },
    }, step);
    return out;
  }
  
  inline std::ostream& operator<<(std::ostream& out, const expr_ast& code) {
    out << "get " << code.global;
    for (auto& i: code.steps) {
      out << " -> " << i;
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