/**
 * @file experiment.cpp
 * @author jgcodes2020
 * @brief A test program that I am using to experiment.
 * @version 0.1
 * @date 2021-05-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH

#include <iostream>
#include <string>
#include <memory>

#include "libdwarf.h"


using std::cout, std::cin, std::flush, std::endl;
using std::string;

int main() {
  string s;
  cout << "Enter a lib path: " << flush;
  std::getline(cin, s);
  cout << "Using lib " << s << endl;

  
}