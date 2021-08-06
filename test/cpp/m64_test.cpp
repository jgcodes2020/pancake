#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>

#include <pancake/movie.hpp>
#include <pancake/sm64.hpp>

#include <debug.hpp>

using namespace pancake;
using namespace std;

std::ostream& operator<<(std::ostream& out, frame& f) {
  out << (((f.buttons & frame::button::A) != frame::button::none)? "A":" ");
  out << (((f.buttons & frame::button::B) != frame::button::none)? "B":" ");
  out << (((f.buttons & frame::button::Z) != frame::button::none)? "Z":" ");
  out << (((f.buttons & frame::button::START) != frame::button::none)? "S":" ");
  out << (((f.buttons & frame::button::D_UP) != frame::button::none)? "\u25B3":" ");
  out << (((f.buttons & frame::button::D_DOWN) != frame::button::none)? "\u25BD":" ");
  out << (((f.buttons & frame::button::D_LEFT) != frame::button::none)? "\u25C1":" ");
  out << (((f.buttons & frame::button::D_RIGHT) != frame::button::none)? "\u25B7":" ");
  out << (((f.buttons & frame::button::U1) != frame::button::none)? ".":" ");
  out << (((f.buttons & frame::button::U2) != frame::button::none)? ".":" ");
  out << (((f.buttons & frame::button::L) != frame::button::none)? "L":" ");
  out << (((f.buttons & frame::button::R) != frame::button::none)? "R":" ");
  out << (((f.buttons & frame::button::C_UP) != frame::button::none)? "\u25B2":" ");
  out << (((f.buttons & frame::button::C_DOWN) != frame::button::none)? "\u25BC":" ");
  out << (((f.buttons & frame::button::C_LEFT) != frame::button::none)? "\u25C0":" ");
  out << (((f.buttons & frame::button::C_RIGHT) != frame::button::none)? "\u25B6":" ");
  return out << " | " << setw(4) << int16_t(f.stick_x) << " " << setw(4) << int16_t(f.stick_y);
}

int main(int argc, char* argv[]) {
  sm64 game(argv[1]);
  m64 inputs(argv[2]);
  const uint32_t float_digits = numeric_limits<double>::max_digits10;
  cout.precision(float_digits);
  
  float max_speed = 0;
  
  for (size_t i = 0; i < inputs.size(); i++) {
    cerr << "\033[2K\033[1E\033[1FFrame: " << i;
    
    inputs[i].apply(game);
    
    game.advance();
    cout << inputs[i] << " | " << game.get<float>("gMarioStates[0].forwardVel") << endl;
    if (abs(game.get<float>("gMarioStates[0].forwardVel")) > abs(max_speed))
      max_speed = game.get<float>("gMarioStates[0].forwardVel");
  }
  cerr << "\n";
  cout << "Maximum speed was: " << max_speed << endl;
}