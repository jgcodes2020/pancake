/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#include <libdwarf/libdwarf.h>
#include "pancake/dwarf/enums.hpp"
#include <pancake/expr/compile.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <pancake/dwarf/type_lookup.hpp>
#include <pancake/dwarf/types.hpp>

using std::stringstream, std::invalid_argument;
using std::string;

namespace {
  void print_ast(
    std::ostream& out, const pancake::expr::expr_ast& ast,
    size_t limit = std::numeric_limits<size_t>::max()) {
    out << ast.global;
    for (size_t i = 0; i < std::min(ast.steps.size(), limit); i++) {
      std::visit(
        stx::overload {
          [&](const pancake::expr::expr_ast::member& step) {
            out << "." << step.name;
          },
          [&](const pancake::expr::expr_ast::subscript& step) {
            out << "[" << step.index << "]";
          }},
        ast.steps[i]);
    }
  }
}  // namespace

namespace pancake::expr {
  const std::pair<expr_eval, dwarf::base_type_info> compile(
    const expr_ast& ast, dwarf::debug& dbg) {
    expr_eval result;

    result.global  = ast.global;
    dwarf::die die = dbg.globals().find(ast.global).value().die();

    // resolve incomplete DIE
    if (die.has_attr(dwarf::dw_attrs::specification))
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::specification);
    // find DIE type
    die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

    if (die.tag() == dwarf::die_tag::typedef_)
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
    // struct deref flag
    bool deref_struct_ptr = false;

    for (size_t i = 0; i < ast.steps.size(); i++) {
      // cerr << "Processing \"" << ast.steps[i] << "\"\n";
      if (die.tag() == dwarf::die_tag::typedef_)
        die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
      visit(
        stx::overload {
          [&](const expr_ast::member& step) mutable -> void {
            if (die.tag() == dwarf::die_tag::pointer_type) {
              die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
              if (!deref_struct_ptr) {
                deref_struct_ptr = true;
              }
              else {
                result.steps.push_back(expr_eval::step(expr_eval::indirect()));
              }
            }
            dwarf::die_tag tag = die.tag();
            // make sure it is a struct or union
            if (
              tag != dwarf::die_tag::structure_type &&
              tag != dwarf::die_tag::union_type) {
              stringstream fmt;
              fmt << "Expected \033[0;38;5;202m";
              print_ast(fmt, ast, i);
              fmt
                << "\033[0m to be a struct or union, actually was \033[0;38;5;38m";
              fmt << tag << "\033[0m";
              throw invalid_argument(fmt.str());
            }

            dwarf::die child = die.child().value();
            bool found;
            do {
              if (
                child.get_attr<std::string>(dwarf::dw_attrs::name) ==
                step.name) {
                found = true;
                break;
              }
            } while ([&]() -> bool {
              auto next = child.sibling();
              if (!next)
                return false;
              child = *next;
              return true;
            }());
            // make sure there is a child
            if (!found) {
              stringstream fmt;
              fmt << "\033[0;38;5;202m" << step.name;
              fmt << "\033[0m is not a member of \033[0;38;5;38m";
              print_ast(fmt, ast, i);
              fmt << "\033[0m";
              throw invalid_argument(fmt.str());
            }
            // cerr << "Found member: " << pdwarf::attr<string>(child,
            // attr_type::name, dbg) << "\n";
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
          [&](const expr_ast::subscript& step) mutable -> void {
            dwarf::die_tag tag = die.tag();
            if (
              tag != dwarf::die_tag::pointer_type &&
              tag != dwarf::die_tag::array_type) {
              stringstream fmt;
              fmt << "Expected \033[0;38;5;202m";
              print_ast(fmt, ast, i);
              fmt
                << "\033[0m to be a pointer or array, actually was \033[0;38;5;38m";
              fmt << tag;
              throw invalid_argument(fmt.str());
            }
            if (die.has_attr(dwarf::dw_attrs::byte_stride)) {
              // use byte stride if available
              result.steps.push_back(
                expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                  die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::byte_stride) *
                  step.index)}));
            }
            else {
              // size of member otherwise
              auto type_die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
              if (type_die.tag() == dwarf::die_tag::typedef_)
                type_die = type_die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

              auto size =
                type_die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::byte_size);
              // cerr << "Size value is " << size << "\n";
              result.steps.push_back(expr_eval::step(
                expr_eval::offset {static_cast<intptr_t>(size * step.index)}));
            }
            die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
            if (die.tag() == dwarf::die_tag::typedef_)
              die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);
          }},
        ast.steps[i]);
    }
    if (die.tag() == dwarf::die_tag::typedef_)
      die = die.get_attr<dwarf::die>(dwarf::dw_attrs::type);

    dwarf::die_tag tag = die.tag();

    // Throw if not base type
    switch (tag) {
      case dwarf::die_tag::base_type:
        break;
      default: {
        stringstream fmt;
        fmt << "Expression \033[0;38;5;202";
        print_ast(fmt, ast);
        fmt
          << "\033[0m does not refer to a base type. "
             "\033[0;1mstruct\033[0ms and \033[0;1munion\033[0ms are currently unsupported.";
        throw std::domain_error(fmt.str());
      }
    }
    
    return std::pair<expr_eval, dwarf::base_type_info> {
      result,
      dwarf::base_type_info {
        static_cast<dwarf::encoding>(
          die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::encoding)),
        die.get_attr<Dwarf_Unsigned>(dwarf::dw_attrs::byte_size)
      }
    };
  }
}  // namespace pancake::expr