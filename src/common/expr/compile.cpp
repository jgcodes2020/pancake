#include <libdwarf/libdwarf.h>
#include "pancake/dwarf/memory.hpp"
#include "pancake/dwarf/type_lookup.hpp"
#include <pancake/expr/compile.hpp>

#include <pancake/dwarf/functions.hpp>
#include <pancake/expr/parse.hpp>
#include <pancake/stx/overload.hpp>
#include <iostream>
#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <utility>

using std::stringstream, std::invalid_argument, std::cerr;
using std::unique_ptr, std::shared_ptr, std::string;

using pdwarf::dw_signed, pdwarf::attr_type, pdwarf::die_tag;

namespace {
  void print_ast(std::ostream& out, const pancake::expr::expr_ast& ast, size_t limit = std::numeric_limits<size_t>::max()) {
    out << ast.global;
    for (size_t i = 0; i < std::min(ast.steps.size(), limit); i++) {
      std::visit(stx::overload {
        [&](const pancake::expr::expr_ast::member& step) {
          out << "." << step.name;
        },
        [&](const pancake::expr::expr_ast::subscript& step) {
          out << "[" << step.index << "]";
        }
      }, ast.steps[i]);
    }
  }
}

namespace pancake::expr {
  const std::pair<expr_eval, const std::type_info&> compile(const expr_ast& ast, shared_ptr<Dwarf_Debug_s>& dbg) {
    expr_eval result;
    
    result.global = ast.global;
    std::unique_ptr<Dwarf_Die_s, pdwarf::dwarf_deleter<Dwarf_Die>> die;
    {
      dw_signed size;
      Dwarf_Global* globals_r = pdwarf::get_globals(dbg, size);
      bool found_global = false;
      auto globals = pdwarf::dwarf_make_unique<Dwarf_Global[]>(
        globals_r, dbg, size
      );
      for (int i = 0; i < size; i++) {
        if (pdwarf::global_name(globals[i], dbg) == ast.global) {
          die.reset(pdwarf::global_die(globals[i], dbg));
          found_global = true;
          break;
        }
      }
      if (!found_global) {
        stringstream fmt;
        fmt << "Global " << ast.global << " is nonexistent";
        throw std::invalid_argument(fmt.str());
      }
    }
    
    // resolve incomplete DIE
    if (pdwarf::has_attr(die, attr_type::specification, dbg))
      die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::specification, dbg));
    // find DIE type
    die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
    
    if (pdwarf::tag(die, dbg) == pdwarf::die_tag::typedef_)
      die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
    // struct deref flag
    bool deref_struct_ptr = false;
    
    for (size_t i = 0; i < ast.steps.size(); i++) {
      //cerr << "Processing \"" << ast.steps[i] << "\"\n";
      if (pdwarf::tag(die, dbg) == pdwarf::die_tag::typedef_)
        die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
      visit(stx::overload {
        [&](const expr_ast::member& step) mutable -> void {
          if (pdwarf::tag(die, dbg) == pdwarf::die_tag::pointer_type) {
            die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
            if (!deref_struct_ptr) {
              deref_struct_ptr = true;
            }
            else {
              result.steps.push_back(expr_eval::step(expr_eval::indirect()));
            }
          }
          die_tag tag = pdwarf::tag(die, dbg);
          // make sure it is a struct or union
          if (tag != die_tag::structure_type && tag != die_tag::union_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202m";
            print_ast(fmt, ast, i);
            fmt << "\033[0m to be a struct or union, actually was \033[0;38;5;38m";
            fmt << tag << "\033[0m";
            throw invalid_argument(fmt.str());
          }
          
          auto child = pdwarf::dwarf_make_unique<Dwarf_Die>(
            pdwarf::child(die, dbg));
          auto set_die = [](decltype(child)& ptr, Dwarf_Die next) {
            if (next == nullptr) return false;
            else {
              ptr.reset(next);
              return true;
            }
          };
          do {
            if (pdwarf::attr<string>(child, attr_type::name, dbg) == step.name)
              break;
          } while (set_die(child, pdwarf::siblingof(child, dbg)));
          // make sure there is a child
          if (child.get() == nullptr) {
            stringstream fmt;
            fmt << "\033[0;38;5;202m" << step.name;
            fmt << "\033[0m is not a member of \033[0;38;5;38m";
            print_ast(fmt, ast, i);
            fmt << "\033[0m";
            throw invalid_argument(fmt.str());
          }
          //cerr << "Found member: " << pdwarf::attr<string>(child, attr_type::name, dbg) << "\n";
          die.reset(pdwarf::attr<Dwarf_Die>(child, attr_type::type, dbg));
          if (tag == die_tag::structure_type) {
            // offset by address of struct member
            intptr_t off = static_cast<intptr_t>(pdwarf::attr<Dwarf_Unsigned>(child, attr_type::data_member_location, dbg));
            if (off != 0) {
              result.steps.push_back(expr_eval::step(expr_eval::offset {off}));
            }
          }
          if (pdwarf::tag(die, dbg) == pdwarf::die_tag::typedef_)
            die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
        },
        [&](const expr_ast::subscript& step) mutable -> void {
          die_tag tag = pdwarf::tag(die, dbg);
          if (tag != die_tag::pointer_type && tag != die_tag::array_type) {
            stringstream fmt;
            fmt << "Expected \033[0;38;5;202m";
            print_ast(fmt, ast, i);
            fmt << "\033[0m to be a pointer or array, actually was \033[0;38;5;38m";
            fmt << tag;
            throw invalid_argument(fmt.str());
          }
          if (pdwarf::has_attr(die, attr_type::byte_stride, dbg)) {
            // use byte stride if available
            result.steps.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                pdwarf::attr<Dwarf_Unsigned>(die, attr_type::byte_stride, dbg) * step.index
              )})
            );
          }
          else {
            // size of member otherwise
            auto type_die = pdwarf::dwarf_make_unique<Dwarf_Die>(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
            if (pdwarf::tag(type_die, dbg) == pdwarf::die_tag::typedef_)
              type_die.reset(pdwarf::attr<Dwarf_Die>(type_die, attr_type::type, dbg));
              
            auto size = pdwarf::attr<Dwarf_Unsigned>(type_die, attr_type::byte_size, dbg);
            //cerr << "Size value is " << size << "\n";
            result.steps.push_back(
              expr_eval::step(expr_eval::offset {static_cast<intptr_t>(
                size * step.index
              )})
            );
          }
          die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
          if (pdwarf::tag(die, dbg) == pdwarf::die_tag::typedef_)
            die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
        }
      }, ast.steps[i]);
    }
    if (pdwarf::tag(die, dbg) == pdwarf::die_tag::typedef_)
      die.reset(pdwarf::attr<Dwarf_Die>(die, attr_type::type, dbg));
    
    die_tag tag = pdwarf::tag(die, dbg);
    
    // Throw if not base type or pointer type
    switch (tag) {
      case die_tag::base_type:
      break;
      default: {
        stringstream fmt;
        fmt << "Expression \033[0;38;5;202";
        print_ast(fmt, ast);
        fmt << "\033[0m does not refer to a base type. "
        "\033[0;1mstruct\033[0ms and \033[0;1munion\033[0ms are currently unsupported.";
        throw std::domain_error(fmt.str());
      }
    }
    
    //cerr << result << "\n";
    return std::pair<expr_eval, const std::type_info&> {
      result, 
      pdwarf::lookup_type(
        static_cast<pdwarf::encoding>(pdwarf::attr<Dwarf_Unsigned>(die, attr_type::encoding, dbg)), 
        pdwarf::attr<Dwarf_Unsigned>(die, attr_type::byte_size, dbg)
      )
    };
  }
}