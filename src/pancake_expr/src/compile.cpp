/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#include <libdwarf/libdwarf.h>
#include "pancake/dwarf/enums.hpp"
#include "pancake/stx/overload.hpp"
#include <pancake/expr/compile.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <variant>
#include <pancake/dwarf/type_info.hpp>
#include <pancake/dwarf/types.hpp>

using std::string;
using std::stringstream, std::invalid_argument;

namespace {
  void print_ast(
    std::ostream& out, const pancake::expr::expr_ast& ast,
    size_t limit = std::numeric_limits<size_t>::max()) {
    out << ast.global;
    for (size_t i = 0; i < std::min(ast.steps.size(), limit); i++) {
      std::visit(
        stx::overload {
          [&](const pancake::expr::expr_ast::dot& step) {
            out << "." << step.name;
          },
          [&](const pancake::expr::expr_ast::arrow& step) {
            out << "->" << step.name;
          },
          [&](const pancake::expr::expr_ast::subscript& step) {
            out << "[" << step.index << "]";
          }},
        ast.steps[i]);
    }
  }
}  // namespace

namespace pancake::expr {
  expr_eval compile(const expr_ast& ast, dwarf::debug& dbg) {
    expr_eval result;

    result.start   = ast.global;
    dwarf::die die = dbg.globals().find(ast.global).value().die();
    
    if (die.has_attr(dwarf::dw_attrs::specification))
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::specification);
    
    if (die.has_attr(dwarf::dw_attrs::type))
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
      
    if (die.tag() == dwarf::die_tag::typedef_)
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
    
    auto& steps = ast.steps;
    for (size_t i = 0; i < steps.size(); i++) {
      std::visit(
        stx::overload {
          [&](const expr_ast::subscript& step) mutable {
            switch (die.tag()) {
              case dwarf::die_tag::pointer_type: {
                result.steps.push_back(expr_eval::indirect {});
              }
              // fall through case here, since logic is basically the same
              case dwarf::die_tag::array_type: {
                if (die.has_attr(dwarf::dw_attrs::byte_stride)) {
                  // use byte stride if available
                  result.steps.push_back(
                    expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                      die.get_attr<Dwarf_Unsigned>(
                        dwarf::dw_attrs::byte_stride) *
                      step.index)}));
                }
                else {
                  // size of member otherwise
                  auto type_die =
                    die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
                  if (type_die.tag() == dwarf::die_tag::typedef_)
                    type_die =
                      type_die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

                  auto size = type_die.get_attr<Dwarf_Unsigned>(
                    dwarf::dw_attrs::byte_size);
                  // cerr << "Size value is " << size << "\n";
                  result.steps.push_back(expr_eval::offset {
                    static_cast<intptr_t>(size * step.index)});
                }
                die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
                if (die.tag() == dwarf::die_tag::typedef_)
                  die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
              } break;
              default: {
                stringstream fmt;
                fmt << "\033[0;38;5;38m";
                print_ast(fmt, ast, i);
                fmt << "\033[0m is not a pointer or array, ";
                fmt << "actually is " << die.tag();
                throw invalid_argument(fmt.str());
              }
            }
          },
          [&](const expr_ast::dot& step) mutable {
            dwarf::die_tag tag = die.tag();
            if (!(tag == dwarf::die_tag::structure_type ||
                  tag == dwarf::die_tag::union_type)) {
              stringstream fmt;
              fmt << "\033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m is not a struct or union";
              throw invalid_argument(fmt.str());
            }

            // Search for the correct member
            dwarf::die child = die.child().value();
            bool found;
            do {
              if (
                child.get_attr<std::string>(dwarf::dw_attrs::name) ==
                step.name) {
                found = true;
                break;
              }
            } while ([&]() mutable -> bool {
              auto next = child.sibling();
              if (!next)
                return false;
              child = *next;
              return true;
            }());
            if (!found) {
              stringstream fmt;
              fmt << "\033[0;38;5;202m" << step.name;
              fmt << "\033[0m is not a member of \033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m";
              throw invalid_argument(fmt.str());
            }

            // member found!
            die = child.get_attr<dwarf::die>(dwarf::dw_attrs::type);
            if (tag == dwarf::die_tag::structure_type) {
              // offset by address of struct member
              intptr_t off =
                static_cast<intptr_t>(child.get_attr<Dwarf_Unsigned>(
                  dwarf::dw_attrs::data_member_location));
              if (off != 0) {
                result.steps.push_back(
                  expr_eval::step(expr_eval::offset {off}));
              }
            }
            if (die.tag() == dwarf::die_tag::typedef_)
              die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
          },
          [&](const expr_ast::arrow& step) mutable {
            if (die.tag() != dwarf::die_tag::pointer_type) {
              stringstream fmt;
              fmt << "\033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m is not a pointer";
              throw invalid_argument(fmt.str());
            }
            result.steps.push_back(expr_eval::indirect {});
            die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

            dwarf::die_tag tag = die.tag();
            if (!(tag == dwarf::die_tag::structure_type ||
                  tag == dwarf::die_tag::union_type)) {
              stringstream fmt;
              fmt << "\033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m is not a pointer to struct or union";
              throw invalid_argument(fmt.str());
            }

            // Search for the correct member
            dwarf::die child = die.child().value();
            bool found;
            do {
              if (
                child.get_attr<std::string>(dwarf::dw_attrs::name) ==
                step.name) {
                found = true;
                break;
              }
            } while ([&]() mutable -> bool {
              auto next = child.sibling();
              if (!next)
                return false;
              child = *next;
              return true;
            }());
            if (!found) {
              stringstream fmt;
              fmt << "\033[0;38;5;202m" << step.name;
              fmt << "\033[0m is not a member of \033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m";
              throw invalid_argument(fmt.str());
            }

            // member found!
            die = child.get_attr<dwarf::die>(dwarf::dw_attrs::type);
            if (tag == dwarf::die_tag::structure_type) {
              // offset by address of struct member
              intptr_t off =
                static_cast<intptr_t>(child.get_attr<Dwarf_Unsigned>(
                  dwarf::dw_attrs::data_member_location));
              if (off != 0) {
                result.steps.push_back(
                  expr_eval::step(expr_eval::offset {off}));
              }
            }
            if (die.tag() == dwarf::die_tag::typedef_)
              die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
          },
        },
        steps[i]);
    }
    if (die.tag() == dwarf::die_tag::typedef_)
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

    dwarf::die_tag tag = die.tag();
    switch (tag) {
      case dwarf::die_tag::base_type: {
        result.result = dwarf::base_type_info {
          static_cast<dwarf::encoding>(
            die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::encoding)),
          die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::byte_size)};
        return result;
      } break;
      default: {
        result.result = dwarf::base_type_info {dwarf::encoding::none, 0};
        return result;
      }
    }
  }
}  // namespace pancake::expr