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

#include "LIEF/LIEF.hpp"


using std::cout, std::cin, std::flush;
using std::string;

using namespace LIEF;

int main() {
  string s;
  cout << "Enter a lib path: ";
  std::getline(cin, s);
  cout << "Using lib " << s << "\n" << flush;

  std::unique_ptr<PE::Binary> bin = PE::Parser::parse(s);

  cout << (*bin) << std::endl;
}