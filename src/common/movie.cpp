#include "pancake/movie.hpp"

#include <ios>
#include <iostream>
#include <fstream>
#include <limits>
#include <locale>
#include <memory>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <filesystem>
#include <vector>

#include "pancake/sm64.hpp"

using std::string;
using std::ios, std::stringstream, std::fstream;
using std::invalid_argument, std::out_of_range;
using std::unique_ptr;
using std::numeric_limits;
namespace fs = std::filesystem;

namespace pancake {
  
  m64::m64(const char* path) :
    m64(string(path)) {}
  
  m64::m64(fs::path path) :
    m64(path.string()) {}
    
  m64::m64(string path) {
    fstream file(path, ios::in | ios::binary);
    if (!file.good()) {
      stringstream fmt;
      fmt << "File " << path << " isn't a file or it doesn't exist";
      throw invalid_argument(fmt.str());
    }
    
    {
      char len_bytes[4];
      // seek to length at 0x0018
      file.seekg(0x0018, ios::beg);
      file.read(len_bytes, 4);
      
      // read as little-endian
      m_inputs_len = 
        ((uint8_t) len_bytes[0]) |
        ((uint8_t) len_bytes[1] << 8) |
        ((uint8_t) len_bytes[2] << 16) |
        ((uint8_t) len_bytes[3] << 24);
    }
    
    m_inputs.reserve(m_inputs_len); {
      std::unique_ptr<char[]> data = unique_ptr<char[]>(new char[4 * m_inputs_len]);
      // seek to inputs at 0x0100
      file.seekg(0x0100, ios::beg);
      file.read(&data[0], 4 * m_inputs_len);
      
      // 4 bytes per frame
      for (uint32_t i = 0; i < m_inputs_len; i++) {
        size_t offset = i * 4;
        m_inputs.push_back(frame {
          static_cast<button>(static_cast<uint16_t>(
            ((uint8_t) data[offset] << 8) |
            ((uint8_t) data[offset + 1]))),
          static_cast<int8_t>(data[offset + 2]),
          static_cast<int8_t>(data[offset + 3])
        });
      }
    }
  }
  
  m64::frame& m64::operator[](uint32_t frame) {
    return m_inputs.at(frame);
  }
  
  m64::frame& m64::front() {
    return m_inputs.front();
  }
  
  m64::frame& m64::back() {
    return m_inputs.back();
  }
  
  uint32_t m64::size() const {
    return m_inputs_len;
  }
  
  m64::iterator m64::begin() {
    return m_inputs.begin();
  }
  
  m64::iterator m64::end() {
    return m_inputs.end();
  }
  
  m64::const_iterator m64::begin() const {
    return m_inputs.begin();
  }
  
  m64::const_iterator m64::end() const {
    return m_inputs.end();
  }
  
  m64::const_iterator m64::cbegin() const {
    return m_inputs.cbegin();
  }
  
  m64::const_iterator m64::cend() const {
    return m_inputs.cend();
  }
  
  void m64::push_back(const frame& frame) {
    if (m_inputs_len == std::numeric_limits<uint32_t>::max()) {
      throw out_of_range("An .m64 cannot be longer than 2^32 - 1 frames");
    }
    m_inputs.push_back(frame);
    m_inputs_len = m_inputs.size();
  }
  
  void m64::push_back(frame&& frame) {
    if (m_inputs_len == std::numeric_limits<uint32_t>::max()) {
      throw out_of_range("An .m64 cannot be longer than 2^32 - 1 frames");
    }
    m_inputs.push_back(frame);
    m_inputs_len = m_inputs.size();
  }
  
  void m64::pop_back() {
    if (m_inputs.size() > 0) {
      m_inputs.pop_back();
      m_inputs_len = m_inputs.size();
    }
    else {
      throw out_of_range("Tried to pop element from empty M64");
    }
  }
  
  void m64::frame::apply(pancake::sm64& game) const {
    game.get<uint16_t>("gControllerPads[0].button") = static_cast<uint16_t>(this->buttons);
    game.get<int8_t>("gControllerPads[0].stick_x") = this->stick_x;
    game.get<int8_t>("gControllerPads[0].stick_y") = this->stick_y;
  }
}