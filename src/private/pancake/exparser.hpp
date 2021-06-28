/**
 * @file exparser.hpp
 * @author jgcodes2020
 * @brief Parser for "C-style accessor expressions"
 * @version 0.1
 * @date 2021-06-18
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_EXPARSER_HPP_
#define _PANCAKE_EXPARSER_HPP_

#include <cstdint>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <any>
#include <vector>
#include <unordered_map>
/**
 * @brief Expression parser producing a list of "instructions."
 */
namespace pancake::exparser {
  enum class opcode : size_t {
    // Represents an invalid opcode
    NONE      = 0,
    // Represents a subscript (array-access) opcode
    SUBSCRIPT = 1,
    // Represents a member access opcode
    MEMBER    = 2
  };
  using instruction = std::pair<opcode, std::any>;
  /**
   * @brief Parsed output. Not really a tree per se, but it may if I expand the syntax.
   * 
   */
  struct ast {
    std::string global;
    std::vector<instruction> instructions;
  };
  
  inline std::ostream& operator<<(std::ostream& out, const ast& code) {
    out << "get " << code.global;
    for (auto& i: code.instructions) {
      out << " -> ";
      switch (i.first) {
        case opcode::SUBSCRIPT: {
          out << "subscript " << std::any_cast<size_t>(i.second);
        } break;
        case opcode::MEMBER: {
          out << "member " << std::any_cast<std::string>(i.second);
        } break;
        default: break;
      }
    }
    return out;
  }
  inline std::ostream& operator<<(std::ostream& out, const opcode& code) {
    switch (code) {
      case opcode::NONE:
        return out << "NO-OP";
      case opcode::SUBSCRIPT:
        return out << "SUBSCRIPT";
      case opcode::MEMBER:
        return out << "MEMBER";
    }
  }
  
  /**
   * @brief Parses an expression into instructions.
   * 
   * @param expr the expression to parse 
   * @return An "access code" containing info on how to retrieve the desired value.
   */
  const ast parse(std::string expr);
  
  /**
   * @brief Uses the macro definitions to preprocess the string.
   * 
   * @param expr the expression to preprocess
   * @return The preprocessed string
   */
  std::string preprocess(std::string expr);
  
  
  class misc_parsing {
  public:
    /**
     * @brief Parses an integer literal.
     * 
     * @param expr 
     * @return const size_t 
     */
    inline static size_t parse_int_literal(std::string expr) {
      if (expr.rfind("0b", 0) || expr.rfind("0B", 0)) {
        return static_cast<size_t>(std::stoll(expr.substr(2)), 2);
      }
      else {
        return static_cast<size_t>(std::stoll(expr));
      }
    }
  };
}
#endif