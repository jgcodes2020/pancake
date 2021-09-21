#include <libdwarf/libdwarf.h>
#include <iostream>
#include <string>
#include "pancake/expr/compile.hpp"
#include <pancake/dwarf/types.hpp>
#include <pancake/expr/parse.hpp>

using std::string, std::cout, std::cin, std::getline;
namespace dwarf = pancake::dwarf;

int main(int argc, char* argv[]) {
  auto dbg = dwarf::debug(argv[1]);
  std::string in;
  
  while (true) {
    cout << "Input an expression: ";
    getline(cin, in);
    if (in == "exit")
      return 0;
    auto p = pancake::expr::parse(in);
    auto c = pancake::expr::compile(p, dbg);
  }
}