/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#include <libdwarf/libdwarf.h>
#include <pancake/expr/parse.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <pancake/macro_defns.hpp>

using std::cerr, std::stringstream, std::vector, std::string_view;
using std::string, std::regex, std::regex_search, std::smatch;

namespace pancake::expr {
  std::ostream& operator<<(std::ostream& out, lexer::token& token) {
    return out << "\"" << token.text << "\"";
  }

  template <typename T>
  std::ostream& operator<<(std::ostream& out, std::vector<T> vec) {
    out << "[";
    for (int i = 0; i < vec.size(); i++) {
      out << vec[i] << ((i + 1 == vec.size()) ? "" : ", ");
    }
    return out << "]";
  }
  // Matches either a [, a ], a ., a ->, an identifier,
  // a decimal literal, a hex literal, a binary literal, or an octal literal.
  inline static const regex EXP_TOKEN =
    regex(R"(^\s*([\[\].]|(?:->)|(?:[A-Za-z_]\w*)|(?:[1-9]?[0-9]+)|)"
          R"((?:\d|[1-9]\d+)|(?:0x[\da-fA-F]+)|(?:0b[01]+)|(?:0[0-7]+)))");

  // Lexes and preprocesses a string.
  vector<lexer::token> preprocess(const string& expr) {
    using lexer::token;

    auto begin = expr.begin();
    smatch match;
    token::type last_type;

    vector<token> result;

    for (; begin < expr.end(); begin += match.length()) {
      bool found_regex = false;
      for (auto it = lexer::type_regexes().begin();
           it != lexer::type_regexes().end(); it++) {
        if (regex_search(begin, expr.end(), match, it->second)) {
          last_type   = it->first;
          found_regex = true;
          break;
        }
      }
      if (!found_regex) {
        stringstream fmt;
        fmt << "Error parsing \"" << expr << "\": invalid token at index "
            << (begin - expr.begin());
        throw std::invalid_argument(fmt.str());
      }
      string_view matched(&expr[0] + (begin - expr.begin()), match.length());
      // cerr << "Parsing token \"" << matched << "\"\n";

      // search object fields
      nlohmann::json::iterator entry;
      nlohmann::json search_space;
      if ([&]() mutable -> bool {
            search_space = sm64_macro_defns::get()["object_fields"];
            entry        = search_space.find(matched);
            return entry != search_space.end();
          }()) {
        auto& obj = *entry;
        result.insert(
          result.end(),
          {token {"rawData", token::type::identifier, begin - expr.begin()},
           token {".", token::type::member_op, begin - expr.begin()},
           token {
             static_cast<string>(obj.at("array")), token::type::identifier,
             begin - expr.begin()}});
        // before indices
        for (auto& i : obj.at("indices")) {
          result.insert(
            result.end(),
            {
              token {"[", token::type::subscript_begin, begin - expr.begin()},
              token {
                std::to_string(ptrdiff_t(i)), token::type::number,
                begin - expr.begin()},
              token {"]", token::type::subscript_end, begin - expr.begin()},
            });
        }
        continue;
      }
      else if ([&]() mutable -> bool {
                 search_space = sm64_macro_defns::get()["constants"];
                 entry        = search_space.find(matched);
                 return entry != search_space.end();
               }()) {
        auto obj    = *entry;
        string type = obj.at("type");
        if (type == "s64") {
          result.push_back(
            {std::to_string(obj.at("value").get<int64_t>()),
             token::type::number, begin - expr.begin()});
        }
        else if (type == "f64") {
          throw std::invalid_argument("Floating point values are not allowed");
        }
        else if (type == "null") {
        }
        throw std::logic_error(
          "Pancake was compiled with a broken sm64_macro_defns.json. "
          "Report this as an issue on the issue tracker.");
      }
      else {
        result.push_back(
          token {string(matched), last_type, begin - expr.begin()});
      }
    }
    // cerr << result << "\n";

    return result;
  }

  // Parses a lexed expression.
  expr_ast parse(const std::vector<lexer::token>& tokens, const string& expr) {
    using lexer::token;
    if (tokens[0].t_type != token::type::identifier) {
      stringstream fmt;
      fmt << "\"" << expr << "\" does not begin with an identifier";
      throw std::invalid_argument(fmt.str());
    }
    expr_ast result {tokens[0].text, vector<expr_ast::step> {}};
    for (size_t i = 1; i < tokens.size();) {
      switch (tokens[i].t_type) {
        case token::type::subscript_begin: {
          if (
            i + 2 >= tokens.size() ||
            tokens[i + 2].t_type != token::type::subscript_end) {
            stringstream fmt;
            fmt << "Error parsing \"" << expr << "\": Bracket at index ";
            fmt << tokens[i].index << " is unclosed";
            throw std::invalid_argument(fmt.str());
          }
          if (tokens[i + 1].t_type != token::type::number) {
            stringstream fmt;
            fmt << "Error parsing \"" << expr << "\": Subscript at index";
            fmt << tokens[i].index << " does not contain an index";
            throw std::invalid_argument(fmt.str());
          }
          result.steps.push_back(
            expr_ast::subscript {std::stoull(tokens[i + 1].text)});
          i += 3;
        } break;
        case token::type::member_op: {
          if (
            i + 1 >= tokens.size() ||
            tokens[i + 1].t_type != token::type::identifier) {
            stringstream fmt;
            fmt << "Error parsing \"" << expr
                << "\": Member operator at index ";
            fmt << tokens[i].index << " does not precede an identifier";
            throw std::invalid_argument(fmt.str());
          }
          result.steps.push_back(expr_ast::member {tokens[i + 1].text});
          i += 2;
        } break;
        default: {
          stringstream fmt;
          fmt << "Error parsing \"" << expr << "\": unexpected token \"";
          fmt << tokens[i].text << "\" at index " << tokens[i].index;
        } break;
      }
    }
    // cerr << result << "\n";
    return result;
  }

  expr_ast parse(std::string expr) { return parse(preprocess(expr), expr); }
}  // namespace pancake::expr