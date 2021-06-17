/*******************************************
This is solely for experimental purposes.
I will be changing this file from time to time.
*******************************************/
#include <iostream>
#include <iomanip>
#include <memory>

#include <LIEF/LIEF.hpp>

using std::cout;
using std::unique_ptr;
using namespace LIEF;

int main(int argc, char* argv[]) {
  unique_ptr<PE::Binary> bin = PE::Parser::parse(argv[1]);
  cout << "Savestate data:\n";
  PE::Section data = bin->get_section(".data");
  PE::Section bss = bin->get_section(".bss");
  
  cout << ".data: " << "RVA: " << std::hex << data.virtual_address() << ", Size: " << data.virtual_size();
  cout << ".bss: " << "RVA: " << std::hex << bss.virtual_address() << ", Size: " << bss.virtual_size();
}