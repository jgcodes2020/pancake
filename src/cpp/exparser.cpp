#include "pancake/exparser.hpp"

#include <any>
#include <cstdint>
#include <regex>
#include <array>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>

#include "pancake/macro_defns.hpp"

using std::string, std::array, std::regex, std::regex_search, std::smatch;
using std::any, std::make_any, std::pair, std::cerr, std::stringstream;

namespace pancake::exparser {
  
  // `^([A-Za-z_]\w*)`
  inline static const regex EXP_GLOBAL = regex(R"(^([A-Za-z_]\w*))");
  
  inline static const array<regex, 2> EXP_OPERATORS {
    // `^(\[\([1-9]?[0-9]+)\])(?:\b|$)` (subscript)
    regex(R"(^\[([1-9]?[0-9]+)\])"), 
    // `^(\\.|->)(?:\\b|$)` (member)
    regex(R"(^(\.|->)([A-Za-z_]\w*))"),
  };
  
  inline static const regex EXP_TOKEN = regex(
    R"(^(\[|\]|\.|(?:->)|(?:[A-Za-z_]\w*)|(?:[1-9]?[0-9]+)))"
  );
  
  string preprocess(const string expr) {
    
    auto begin = expr.begin();
    smatch match;
    
    stringstream out;
    
    for (; begin < expr.end(); begin += match.length()) {
      if (!regex_search(begin, expr.end(), match, EXP_TOKEN)) {
        stringstream fmt;
        fmt << "Failed preprocessing expression " << expr;
        fmt << " at column " << begin - expr.begin();
        fmt << ": Did not start with a valid token";
        throw std::invalid_argument(fmt.str());
      }
      
      string matched = match.str(1);
      auto obj_fields = sm64_macro_defns::get()["object_fields"];
      auto sub = obj_fields.find(matched);
      if (sub == obj_fields.end()) {
        out << matched;
      }
      else {
        nlohmann::json obj = *sub;
        out << "rawData." << static_cast<string>(obj["array"]);
        for (auto& i : obj["indices"]) {
          out << '[' << i << ']';
        }
      }
    }
    return out.str();
  }
  
  const ast parse(const string expr) {
    ast result;
    auto begin = expr.begin();
    smatch match;
    // Find global first
    if (!regex_search(begin, expr.end(), match, EXP_GLOBAL))
      throw std::invalid_argument("Expression must begin with an identifier");
    result.global = match.str(1);
    begin += match.length();
    // read tokens until one is invalid or the whole string is read
    for (; begin < expr.end(); begin += match.length()) {
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
          any name = make_any<string>(match.str(2));
          result.instructions.push_back(pair {opcode, name});
        } break;
        case opcode::SUBSCRIPT: {
          any index = make_any<size_t>(std::stoll(match.str(1)));
          result.instructions.push_back(pair {opcode, index});
        } break;
        default: {
          stringstream fmt;
          fmt << expr << " is not a valid expression";
          throw std::invalid_argument(fmt.str());
        } break;
      }
    }
    return result;
  }
}