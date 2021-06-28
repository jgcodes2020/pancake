#include "pancake/movie.hpp"

#include <ios>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>

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
          .buttons = static_cast<uint16_t>(
            ((uint8_t) data[offset] << 8) |
            ((uint8_t) data[offset + 1])),
          .stick_x = static_cast<int8_t>(data[offset + 2]),
          .stick_y = static_cast<int8_t>(data[offset + 3])
        });
      }
    }
    
    m_len = len;
  }
  
  const m64::frame& m64::operator[](uint32_t frame) const {
    return m_inputs.at(frame);
  }
  
  uint32_t m64::size() const {
    return m_len;
  }
  
  auto m64::cbegin() const {
    return m_inputs.cbegin();
  }
  
  auto m64::cend() const {
    return m_inputs.cend();
  }
}