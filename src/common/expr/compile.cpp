#include "pancake/expr/compile.hpp"

#include "pancake/dwarf.hpp"
#include "pancake/expr/parse.hpp"
#include "pancake/overload.hpp"
#include <sstream>
#include <stdexcept>
#include <vcruntime.h>

using namespace dwarf;

using std::stringstream, std::invalid_argument;

namespace pancake::expr {
  const expr_eval compile(expr_ast& ast, dw_debug& dbg) {
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
    
    for (size_t i = 0; i < ast.steps.size(); i++) {
      visit(overload {
        [&](const expr_ast::member& step) mutable -> void {
          dw_die actual = die;
          if (actual.tag() == DW_TAG::pointer_type) {
            actual = actual.attr(DW_AT::type).as_linked_die();
            // need to deref the pointer
            result.offsets.push_back(
              expr_eval::step(expr_eval::indirect{})
            );
          }
          DW_TAG tag = actual.tag();
          // make sure it is a struct or union
          if (tag != DW_TAG::structure_type && tag != DW_TAG::union_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202" << ast.global;
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
            fmt << "\033[0m to be a struct or union, actually was ";
            fmt << tag;
            throw invalid_argument(fmt.str());
          }
          
          dw_die child = actual.first_child();
          do {
            if (child.attr(DW_AT::name).as_string() == step.name)
              break;
          } while ((child = child.sibling()));
          // make sure there is a child
          if (child.is_null()) {
            stringstream fmt;
            fmt << "\033[0;38;5;202" << step.name;
            fmt << "\033[0m is not a member of \033[0;38;5;202";
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
          die = child.attr(DW_AT::type).as_linked_die();
          if (tag == DW_TAG::structure_type) {
            // offset by address of struct member
            result.offsets.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                child.attr(DW_AT::data_member_location).as_unsigned_int()
              )})
            );
          }
        },
        [&](const expr_ast::subscript& step) mutable -> void {
          DW_TAG tag = die.tag();
          if (tag != DW_TAG::pointer_type && tag != DW_TAG::array_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202" << ast.global;
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
            fmt << "\033[0m to be a pointer or array, actually was ";
            fmt << tag;
            throw invalid_argument(fmt.str());
          }
          if (die.has_attr(DW_AT::byte_stride)) {
            // use byte stride if available
            result.offsets.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                die.attr(DW_AT::byte_stride).as_unsigned_int() * step.index
              )})
            );
          }
          else {
            // size of member otherwise
            result.offsets.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                die.attr(DW_AT::type).as_linked_die()
                .attr(DW_AT::byte_size).as_unsigned_int() * step.index
              )})
            );
          }
          result.offsets.push_back(
            expr_eval::step(expr_eval::indirect{})
          );
        }
      }, ast.steps[i]);
    }
    return result;
  }
}