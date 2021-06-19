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
  struct access_code {
    std::string global;
    std::vector<instruction> instructions;
  };
  
  /**
   * @brief Parses an expression into instructions.
   * 
   * @param expr the expression to parse 
   * @return An "access code" containing info on how to retrieve the desired value.
   */
  const access_code parse(std::string expr);
  
  
  class misc_parsing {
  private:
    inline static size_t digit(char c, size_t max) {
      if (max > 36) {
        std::stringstream fmt;
        fmt << "Attempted to parse base-" << max << ", the max is base-36";
        throw std::invalid_argument(fmt.str());
      }
      size_t result = 0;
      if (c >= '0' && c <= '9') {
        // subtract 0x30
        result = static_cast<size_t>(
          static_cast<uint8_t>(c) - 48
        );
      }
      else if (c >= 'A' && c <= 'Z') {
        // subtract 0x41 then add 10
        result = static_cast<size_t>(
          static_cast<uint8_t>(c) - 55
        );
      }
      else if (c >= 'a' && c <= 'z') {
        // subtract 0x61 then add 10
        result = static_cast<size_t>(
          static_cast<uint8_t>(c) - 87
        );
      }
      else {
        std::stringstream fmt;
        fmt << c << " is not a valid digit";
        throw std::invalid_argument(fmt.str());
      }
      
      if (result > max) {
        std::stringstream fmt;
        fmt << c << " is not a valid digit in base-" << max;
        throw std::invalid_argument(fmt.str());
      }
    }
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