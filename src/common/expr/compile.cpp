#include "pancake/expr/compile.hpp"

#include "pancake/dwarf.hpp"
#include "pancake/expr/parse.hpp"
#include "pancake/overload.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vcruntime.h>

using namespace dwarf;

using std::stringstream, std::invalid_argument, std::cerr;

namespace pancake::expr {
  const expr_eval compile(const expr_ast& ast, dw_debug& dbg) {
    expr_eval result;
    
    result.global = ast.global;
    
    dw_die die = dbg.list_globals()[ast.global].as_die();
    // resolve incomplete DIE
    if (die.has_attr(DW_AT::specification))
      die = die.attr(DW_AT::specification).as_linked_die();
    // find DIE type
    die = die.attr(DW_AT::type).as_linked_die();
    
    if (die.tag() == DW_TAG::typedef_)
      die = die.attr(DW_AT::type).as_linked_die();
    // throw for function DIE
    if (die.tag() == DW_TAG::subprogram) {
      stringstream fmt;
      fmt << "\033[0;38;5;202m" << ast.global;
      fmt << "\033[0m is a function pointer and should not be accessed";
    }
    for (size_t i = 0; i < ast.steps.size(); i++) {
      if (die.tag() == DW_TAG::typedef_)
        die = die.attr(DW_AT::type).as_linked_die();
      visit(overload {
        [&](const expr_ast::member& step) mutable -> void {
          if (die.tag() == DW_TAG::pointer_type) {
            die = die.attr(DW_AT::type).as_linked_die();
          }
          DW_TAG tag = die.tag();
          // make sure it is a struct or union
          if (tag != DW_TAG::structure_type && tag != DW_TAG::union_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202m" << ast.global;
            for (size_t j = 0; j < i; j++) {
              visit(overload {
                [&](const expr_ast::member& step) {
                  fmt << "." << step.name;
                },
                [&](const expr_ast::subscript& step) {
                  fmt << "[" << step.index << "]";
                }
              }, ast.steps[j]);
            }
            fmt << "\033[0m to be a struct or union, actually was \033[0;38;5;38m";
            fmt << tag << "\033[0m";
            throw invalid_argument(fmt.str());
          }
          result.steps.push_back(expr_eval::step(expr_eval::indirect()));
          
          dw_die child = die.first_child();
          do {
            if (child.attr(DW_AT::name).as_string() == step.name)
              break;
          } while ((child = child.sibling()));
          // make sure there is a child
          if (child.is_null()) {
            stringstream fmt;
            fmt << "\033[0;38;5;202m" << step.name;
            fmt << "\033[0m is not a member of \033[0;38;5;38m";
            for (size_t j = 0; j < i; j++) {
              visit(overload {
                [&](const expr_ast::member& step) {
                  fmt << "." << step.name;
                },
                [&](const expr_ast::subscript& step) {
                  fmt << "[" << step.index << "]";
                }
              }, ast.steps[j]);
            }
            fmt << "\033[0m";
            throw invalid_argument(fmt.str());
          }
          cerr << "Member name: " << child.attr(DW_AT::name).as_string() << "\n";
          die = child.attr(DW_AT::type).as_linked_die();
          if (tag == DW_TAG::structure_type) {
            // offset by address of struct member
            result.steps.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                child.attr(DW_AT::data_member_location).as_unsigned_int()
              )})
            );
          }
          if (die.tag() == DW_TAG::typedef_)
            die = die.attr(DW_AT::type).as_linked_die();
        },
        [&](const expr_ast::subscript& step) mutable -> void {
          DW_TAG tag = die.tag();
          if (tag != DW_TAG::pointer_type && tag != DW_TAG::array_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202m" << ast.global;
            for (size_t j = 0; j < i; j++) {
              visit(overload {
                [&](const expr_ast::member& step) {
                  fmt << "." << step.name;
                },
                [&](const expr_ast::subscript& step) {
                  fmt << "[" << step.index << "]";
                }
              }, ast.steps[j]);
            }
            fmt << "\033[0m to be a pointer or array, actually was \033[0;38;5;38m";
            fmt << tag;
            throw invalid_argument(fmt.str());
          }
          if (die.has_attr(DW_AT::byte_stride)) {
            // use byte stride if available
            result.steps.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                die.attr(DW_AT::byte_stride).as_unsigned_int() * step.index
              )})
            );
          }
          else {
            // size of member otherwise
            dw_die type_die = die.attr(DW_AT::type).as_linked_die();
            if (type_die.tag() == DW_TAG::typedef_)
              type_die = type_die.attr(DW_AT::type).as_linked_die();
            result.steps.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                type_die.attr(DW_AT::byte_size).as_unsigned_int() * step.index
              )})
            );
          }
          die = die.attr(DW_AT::type).as_linked_die();
          if (die.tag() == DW_TAG::typedef_)
            die = die.attr(DW_AT::type).as_linked_die();
        }
      }, ast.steps[i]);
    }
    if (die.tag() == DW_TAG::typedef_)
      die = die.attr(DW_AT::type).as_linked_die();
    
    DW_TAG tag = die.tag();
    
    // Throw if not base type or pointer type
    switch (tag) {
      case DW_TAG::base_type:
      case DW_TAG::pointer_type: 
      break;
      default: {
        stringstream fmt;
        fmt << "Expression \033[0;38;5;202" << ast.global;
        for (size_t i = 0; i < ast.steps.size(); i++) {
          visit(overload {
            [&](const expr_ast::member& step) {
              fmt << "." << step.name;
            },
            [&](const expr_ast::subscript& step) {
              fmt << "[" << step.index << "]";
            }
          }, ast.steps[i]);
        }
        fmt << "\033[0m does not refer to a base type or pointer. "
        "\033[0;1mstruct\033[0ms and \033[0;1munion\033[0ms are currently unsupported.";
        throw std::domain_error(fmt.str());
      }
    }
    
    return result;
  }
}