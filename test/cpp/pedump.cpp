/*******************************************
This is solely for experimental purposes.
I will be changing this file from time to time.
*******************************************/
#include <cstdlib>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <string>
#include <iomanip>
#include <memory>

#include <stdlib.h>

#include <tuple>
#include <type_traits>
#include <windows.h>

#include <LIEF/LIEF.hpp>

#include "pancake/sm64.hpp"
#include "pancake/movie.hpp"
#include "pancake/dwarf.hpp"

using namespace std;
using namespace pancake;
using namespace LIEF;

#include <debug.hpp>

const constexpr uint32_t SIZEOF_SYMBOL = 18;

template<typename CharT = char>
uint32_t chars_to_uint32(CharT bytes[4], bool use_bigendian) {
  static_assert(
    std::disjunction<
      std::is_same<CharT, char>,
      std::is_same<CharT, uint8_t>
    >::value, 
    "message"
  );
  if (use_bigendian) {
    return (
      static_cast<uint8_t>(bytes[0]) << 24 |
      static_cast<uint8_t>(bytes[1]) << 16 |
      static_cast<uint8_t>(bytes[2]) << 8  |
      static_cast<uint8_t>(bytes[3])
    );
  }
  else {
    return (
      static_cast<uint8_t>(bytes[3]) << 24 |
      static_cast<uint8_t>(bytes[2]) << 16 |
      static_cast<uint8_t>(bytes[1]) << 8  |
      static_cast<uint8_t>(bytes[0])
    );
  }
}

int main(int argc, char* argv[]) {
  // Register handlers
  debug_handlers();
  
  unique_ptr<PE::Binary> bin = PE::Parser::parse(argv[1]);
  PE::Header hdr = bin->header();
  uint32_t sym_table_off = hdr.pointerto_symbol_table();
  uint32_t str_table_off = sym_table_off + (SIZEOF_SYMBOL * hdr.numberof_symbols());
  vector<char> str_table; {
    fstream binfile = fstream(argv[1], ios::in | ios::binary);
    binfile.seekg(str_table_off, ios::beg);
    
    char data[4];
    binfile.read(data, 4);
    uint32_t size = chars_to_uint32(data, false);
    
    str_table.resize(size, 0);
    binfile.seekg(str_table_off, ios::beg);
    binfile.read(&str_table[0], size);
  }
  cout << "Section list: \n";
  for (auto& section: bin->sections()) {
    string name = section.name();
    if (name[0] == '/') {
      uint32_t off = stoul(name.substr(1));
      name = string(&str_table[off]);
    }
    
    cout << "Section " << setw(16) << name;
    cout << ": Address " << hex << uppercase << setw(8) << section.virtual_address();
    cout << ", Size " << hex << uppercase << setw(8) << section.virtual_size() << '\n';
  }
}