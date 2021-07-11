#include "pancake/expr/parse.hpp"

#include <cstdint>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>

#include "pancake/macro_defns.hpp"

using std::string, std::regex, std::regex_search, std::smatch;
using std::cerr, std::stringstream;

namespace pancake::expr {
  
  // Identifiers in C can contain letters, numbers, and underscores. (\w)
  // However, they cannot start with a number. This matches the first char
  // using [A-Za-z_], which is just \w minus the numbers.
  inline static const regex EXP_IDENTIFIER = regex(R"(^([A-Za-z_]\w*))");
  // Matches an opening square bracket first, followed by an arbitrary numeric literal.
  // These are in the order: decimal, hexadecimal, binary, octal
  inline static const regex EXP_SUBSCRIPT = regex(R"(^\[((?:\d|[1-9]\d+)|(?:0x[\da-fA-F]+)|(?:0b[01]+)|(?:0[0-7]+))\])");
  // Same as the global identifier regex, but with a . or -> tacked on in front.
  inline static const regex EXP_MEMBER = regex(R"(^(?:\.|->)([A-Za-z_]\w*))");
  // Matches either a [, a ], a ., a ->, an identifier,
  // a decimal literal, a hex literal, a binary literal, or an octal literal.
  inline static const regex EXP_TOKEN = regex(
    R"(^\s*(\[|\]|\.|(?:->)|(?:[A-Za-z_]\w*)|(?:[1-9]?[0-9]+)|)"
    R"((?:\d|[1-9]\d+)|(?:0x[\da-fA-F]+)|(?:0b[01]+)|(?:0[0-7]+)))"
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
        fmt << ": No valid token";
        throw std::invalid_argument(fmt.str());
      }
      
      string matched = match.str(1);
      cerr << "Parsing token \"" << matched << "\"\n";
      
      // search object fields
      nlohmann::json::iterator entry;
      if ([&]() mutable -> bool {
        auto search_space = sm64_macro_defns::get()["object_fields"];
        entry = search_space.find(matched);
        return entry != search_space.end();
      }()) {
        auto obj = *entry;
        out << "rawData." << static_cast<string>(obj.at("array"));
        for (auto& i : obj.at("indices")) {
          out << '[' << i << ']';
        }
        continue;
      }
      else if ([&]() mutable -> bool {
        auto search_space = sm64_macro_defns::get()["constants"];;
        entry = search_space.find(matched);
        return entry != search_space.end();
      }()) {
        auto obj = *entry;
        string type = obj.at("type");
        if (type == "s64") {
          out << obj.at("value").get<intmax_t>();
        }
        else if (type == "f64") {
          out << obj.at("value").get<double>();
        }
        else if (type == "null") {}
        throw std::runtime_error(
          "Pancake was compiled with a broken sm64_macro_defns.json. "
          "Report this as an issue on the issue tracker."
        );
      }
      else {
        out << matched;
      }
    }
    cerr << expr << " -> " << out.str() << "\n";
    return out.str();
  }
  
  const expr_ast parse(const string expr) {
    expr_ast result;
    auto begin = expr.begin();
    smatch match;
    // Find global first
    if (!regex_search(begin, expr.end(), match, EXP_IDENTIFIER)) {
      stringstream fmt;
      fmt << "Expression \"" << expr << "\" does not begin with a global";
      throw std::invalid_argument(fmt.str());
    }
    result.global = match.str(1);
    begin += match.length();
    // read tokens until one is invalid or the whole string is read
    for (; begin < expr.end(); begin += match.length()) {
      cerr << "Testing against \"" << string(begin, expr.end()) << "\"\n";
      if (regex_search(begin, expr.end(), match, EXP_SUBSCRIPT)) {
        expr_ast::subscript op {
          util::parse_int_literal(match.str(1))
        };
        result.steps.push_back(expr_ast::step(op));
      }
      else if (regex_search(begin, expr.end(), match, EXP_MEMBER)) {
        expr_ast::member op {
          match.str(1)
        };
        result.steps.push_back(expr_ast::step(op));
      }
      else {
        stringstream fmt;
        fmt << "Failed to parse \"" << expr << "\" at index " << begin - expr.begin();
        fmt << ": Could not match a token";
        throw std::invalid_argument(fmt.str());
      }
    }
    cerr << expr << " -> " << result << "\n";
    
    return result;
  }
}