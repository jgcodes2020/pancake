/*******************************************
This is solely for experimental purposes.
I will be changing this file from time to time.
*******************************************/
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <iomanip>
#include <memory>

#include <stdlib.h>

#include <tuple>
#include <windows.h>

#include <DbgHelp.h>
#include <signal.h>

#include "libdwarf.h"
#include "pancake/exparser.hpp"
#include "pancake/dwarf.hpp"

using namespace std;
using namespace pancake;
using namespace dwarf;

#include <debug.hpp>

int main(int argc, char* argv[]) {
  // Register handlers
  debug_handlers();
  
  dw_debug dbg(argv[1]);
  cout << "Searching for gMarioStates...\n";
  // And you thought IIFEs were only for JS?
  dw_die die_gMarioStates = dbg.list_globals()["gMarioStates"];
  
  auto e = die_gMarioStates
    .attr(DW_AT::specification).as_linked_die()
    .attr(DW_AT::type).as_linked_die()
    .attr(DW_AT::type).as_linked_die()
    .attr(DW_AT::name).as_string();
  cout << "gMarioStates die has tag: " << e << "\n";
  return 0;
}