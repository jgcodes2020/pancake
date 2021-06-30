/*******************************************
This is solely for experimental purposes.
I will be changing this file from time to time.
*******************************************/
#include <cstdlib>
#include <cmath>
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
#include "pancake/movie.hpp"
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
  m64 one_key(argv[2]);
  
  uint16_t& level = game.get<uint16_t>("gCurrLevelNum");
  
  for (size_t i = 0; i <= 3286; i++) {
    one_key[i].apply(game);
    game.advance();
    cout << "======== Frame " << i << " ========\n";
    cout << "Level: " << level << "\n";
  }
  
  return 0;
}