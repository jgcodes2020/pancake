/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DEBUG_PAUSE_HPP_
#define _PANCAKE_DEBUG_PAUSE_HPP_

#include <string>
#include <stdio.h>
#if defined(_WIN32)
  #include <conio.h>
#elif defined(__linux__) | defined(__APPLE__)
  #include <termios.h>
  #include <unistd.h>
#endif

namespace debug {
#if defined(_WIN32)
  inline void pause(const char* msg = "Press any key to continue...") {
    puts(msg);
    _getch();
  }
#elif defined(__linux__) | defined(__APPLE__)
  inline void pause(const char* msg = "Press any key to continue...") {
    termios tdata_old, tdata_new;
    puts(msg);
    // Disable canonical input and echo
    tcgetattr(STDIN_FILENO, &tdata_old);
    tdata_new = tdata_old;
    tdata_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tdata_new);
    // Read 1 char
    getchar();
    // Reset values
    tcsetattr(STDIN_FILENO, TCSANOW, &tdata_old);
  }
#endif
  inline void pause(std::string& msg) { pause(msg.c_str()); }
}  // namespace debug
#endif