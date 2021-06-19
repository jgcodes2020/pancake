#include "pancake/exparser.hpp"

#include <any>
#include <cstdint>
#include <regex>
#include <array>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using std::string, std::array, std::regex, std::regex_search, std::smatch;
using std::any, std::make_any, std::pair, std::cerr;

namespace pancake::exparser {
  
  // `^([A-Za-z_]\w*)`
  inline static const regex EXP_GLOBAL = regex("^([A-Za-z_]\\w*)");
  
  inline static const array<regex, 2> EXP_OPERATORS {
    // `^(\[\([1-9]?[0-9]+)\])(?:\b|$)` (subscript)
    regex("^\\[([1-9]?[0-9]+)\\]"), 
    // `^(\\.|->)(?:\\b|$)` (member)
    regex("^(?:\\.|->)([A-Za-z_]\\w*)"),
  };
  
  const access_code parse(const std::string expr) {
    access_code result;
    auto begin = expr.begin();
    smatch match;
    // Find global first
    if (!regex_search(begin, expr.end(), match, EXP_GLOBAL))
      throw std::invalid_argument("Expression must begin with an identifier");
    result.global = match.str(1);
    begin += match.length();
    // read tokens until one is invalid or the whole string is read
    for (; begin != expr.end(); begin += match.length()) {
      opcode opcode = opcode::NONE;
      cerr << "Testing against \"" << string(begin, expr.end()) << "\"\n";
      for (size_t i = 0; i < EXP_OPERATORS.size(); i++) {
        if (regex_search(begin, expr.end(), match, EXP_OPERATORS[i])) {
          opcode = static_cast<enum opcode>(i + 1);
          cerr << "Matched " << opcode << " opcode\n";
          break;
        }
      }
      
      switch (opcode) {
        case opcode::MEMBER: {
          any name = make_any<string>(match.str(1));
          result.instructions.push_back(pair {opcode, name});
        } break;
        case opcode::SUBSCRIPT: {
          any index = make_any<size_t>(std::stoll(match.str(1)));
          result.instructions.push_back(pair {opcode, index});
        } break;
        default: {
          std::stringstream fmt;
          fmt << expr << " is not a valid expression";
          throw std::invalid_argument(fmt.str());
        } break;
      }
    }
    return result;
  }
}