#include "debug.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>

#include "pancake/movie.hpp"
#include "pancake/sm64.hpp"

using namespace pancake;
using namespace std;

int main(int argc, char* argv[]) {
  debug_handlers();
  
  cout << "I swear it should be fine";
  
  sm64 game(argv[1]);
  m64 inputs(argv[2]);
  const uint32_t float_digits = numeric_limits<double>::max_digits10;
  
  cout << "Everything is fine\n" << endl;
  // cout.precision(float_digits);
  
  // float max_speed = 0;
  
  // for (size_t i = 0; i < inputs.size(); i++) {
  //   inputs[i].apply(game);
  //   game.advance();
  //   cout << setprecision(float_digits) << game.get<float>("gMarioStates[0].forwardVel") << endl;
  //   if (abs(game.get<float>("gMarioStates[0].forwardVel")) > abs(max_speed))
  //     max_speed = game.get<float>("gMarioStates[0].forwardVel");
  // }
  // cout << "Maximum speed was: " << max_speed;
  // cout << "Current level is: " << game.get<uint16_t>("gCurrLevelNum");
}