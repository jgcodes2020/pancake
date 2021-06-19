/*******************************************
This is solely for experimental purposes.
I will be changing this file from time to time.
*******************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include <memory>
#include <cstdlib>

#include "pancake/exparser.hpp"

using std::cout, std::cin, std::cerr, std::string;
using std::unique_ptr;
using namespace pancake;

int main(int argc, char* argv[]) {
  try {
    string expr = "";
    cout << "Enter an expression: ";
    std::getline(cin, expr);
    while (expr != "exit") {
      cout << exparser::parse(expr) << "\n";
      cout << "Enter an expression: ";
      std::getline(cin, expr);
    }
  }
  catch (std::exception& e) {
    cerr << "\n\033[0;91m";
    cerr << "Exception of type " << typeid(e).name() << " thrown\n";
    cerr << "what(): " << e.what() << "\n";
    cerr << "\033[0m";
    std::exit(1);
  }
}