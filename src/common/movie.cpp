#include "pancake/movie.hpp"

#include <ios>
#include <iostream>
#include <fstream>
#include <memory>
#include <stdint.h>
#include <string>
#include <filesystem>
#include <vector>
#include <utility>

#include "pancake/sm64.hpp"

using namespace std;
namespace fs = std::filesystem;

namespace pancake {
  m64::m64(const char* path) :
    m64(string(path)) {}
  
  m64::m64(fs::path path) :
    m64(path.string()) {}
    
  m64::m64(string path) {
    fstream file(path, ios::in | ios::binary);
    
    /*const*/ uint32_t len; {
      char len_bytes[4];
      // seek to length at 0x0018
      file.seekg(0x0018, ios::beg);
      file.read(len_bytes, 4);
      
      // read as little-endian
      len = 
        ((uint8_t) len_bytes[0]) |
        ((uint8_t) len_bytes[1] << 8) |
        ((uint8_t) len_bytes[2] << 16) |
        ((uint8_t) len_bytes[3] << 24);
    }
    
    m_inputs.reserve(len); {
      std::unique_ptr<char[]> data = unique_ptr<char[]>(new char[len * 4]);
      // seek to inputs at 0x0100
      file.seekg(0x0100, ios::beg);
      file.read(&data[0], 4 * len);
      
      // 4 bytes per frame
      for (uint32_t i = 0; i < len; i++) {
        size_t offset = i * 4;
        m_inputs.push_back(frame {
          static_cast<uint16_t>(
            ((uint8_t) data[offset] << 8) |
            ((uint8_t) data[offset + 1])),
          static_cast<int8_t>(data[offset + 2]),
          static_cast<int8_t>(data[offset + 3])
        });
      }
    }
    
    m_len = len;
  }
  
  m64::frame& m64::operator[](uint32_t frame) {
    return m_inputs.at(frame);
  }
  
  uint32_t m64::size() const {
    return m_len;
  }
  
  auto m64::begin() const {
    return m_inputs.begin();
  }
  
  auto m64::end() const {
    return m_inputs.end();
  }
  
  void m64::frame::apply(pancake::sm64& game) const {
    game.get<uint16_t>("gControllerPads[0].button") = this->buttons;
    game.get<int8_t>("gControllerPads[0].stick_x") = this->stick_x;
    game.get<int8_t>("gControllerPads[0].stick_y") = this->stick_y;
  }
}