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
#include "coffi/coffi_types.hpp"
#include <stdexcept>
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>

#include <coffi/coffi.hpp>

using std::cout, std::cin;
using std::flush, std::endl, std::setfill, std::setw,
  std::hex, std::left, std::right;
using std::string;

using namespace COFFI;

int main(int argc, char* argv[]) {
  coffi exe;

  if (!exe.load(argv[1])) {
    throw std::invalid_argument("no file");
  }

  if (exe.get_architecture() != coffi_architecture_t::COFFI_ARCHITECTURE_PE) {
    throw std::invalid_argument("file must be a PE");
  }

  cout << setfill(' ');
  for (auto& sect: exe.get_sections()) {
    std::ios::fmtflags flags = cout.flags();
    cout << right << setw(10) << hex << sect->get_virtual_address();
    cout.flags(flags);
    cout << " " << sect->get_name() << endl;
  }
  return 0;
}