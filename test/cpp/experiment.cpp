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
#include "pancake/sm64.hpp"
#include "pancake/exparser.hpp"
#include "pancake/dwarf.hpp"

using namespace std;
using namespace pancake;
using namespace dwarf;

#include <debug.hpp>

int main(int argc, char* argv[]) {
  // Register handlers
  debug_handlers();
  
  sm64 game(argv[1]);
  
  return 0;
}