/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/


#include "pancake/movie.hpp"

#include <cstring>
#include <stdint.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <ios>
#include <iostream>
#include <limits>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <pancake/sm64.hpp>
#include <pancake/debug/pause.hpp>

using std::ios, std::stringstream, std::fstream;
using std::numeric_limits;
using std::out_of_range;
using std::string;
using std::unique_ptr;
template <typename T>
using init_list = std::initializer_list<T>;
namespace fs    = std::filesystem;

namespace {
  const init_list<char> M64_SIG {'M', '6', '4', '\x1A'};

  // prevents using decls
  struct M64_OFFSETS {
    M64_OFFSETS() = delete;

    static const uint16_t signature        = 0x0000;
    static const uint16_t version          = 0x0004;
    static const uint16_t timestamp        = 0x0008;
    static const uint16_t num_vis          = 0x000C;
    static const uint16_t rerecords        = 0x0010;
    static const uint16_t vis_per_s        = 0x0014;
    static const uint16_t num_controllers  = 0x0015;
    static const uint16_t num_input_frames = 0x0018;
    static const uint16_t start_type       = 0x001C;
    static const uint16_t controllers      = 0x0020;
    static const uint16_t rom_name         = 0x00C4;
    static const uint16_t crc              = 0x00E4;
    static const uint16_t country_code     = 0x00E8;
    static const uint16_t video_plugin     = 0x0122;
    static const uint16_t sound_plugin     = 0x0162;
    static const uint16_t input_plugin     = 0x01A2;
    static const uint16_t rsp_plugin       = 0x01E2;
    static const uint16_t authors          = 0x0222;
    static const uint16_t description      = 0x0300;

    static const uint16_t start_of_data = 0x0400;
  };
}  // namespace

namespace pancake {

  m64::m64(fs::path path) :
    m_inputs(1) {
    std::cerr << "Path is " << fs::absolute(path) << "\n";
    std::ifstream in(fs::absolute(path), ios::in | ios::binary);
    if (!in.good()) {
      stringstream fmt;
      fmt << path << " isn't a file or it doesn't exist";
      throw invalid_m64(fmt.str());
    }
    
    static void (*read_int32)(const char*, uint32_t&) = 
      [](const char* p, uint32_t& o) {
      o = static_cast<uint8_t>(p[0]);
      o |= static_cast<uint8_t>(p[1]) << 8;
      o |= static_cast<uint8_t>(p[2]) << 16;
      o |= static_cast<uint8_t>(p[3]) << 24;
    };
    static void(*read_int16)(const char*, uint16_t&) = [](const char* p, uint16_t& o) {
      o = static_cast<uint8_t>(p[0]);
      o |= static_cast<uint8_t>(p[1]) << 8;
    };
    static void(*read_str)(const char*, string&) = [](const char* p, string& o) {
      o = p;
    };
    {
      std::unique_ptr<char[]> data = unique_ptr<char[]>(new char[1024]);
      in.read(&data[0], 1024);
      
      if (!std::equal(M64_SIG.begin(), M64_SIG.end(), &data[0])) {
        stringstream fmt;
        fmt << "File " << path << " isn't a valid M64, signature should be \"M64\\x1A\"";
        throw invalid_m64(fmt.str());
      }
      read_int32(&data[M64_OFFSETS::version], metadata.version);
      read_int32(&data[M64_OFFSETS::timestamp], metadata.timestamp);
      read_int32(&data[M64_OFFSETS::num_vis], metadata.num_vis);
      read_int32(&data[M64_OFFSETS::rerecords], metadata.rerecords);
      metadata.vis_per_s = data[M64_OFFSETS::vis_per_s];
      metadata.num_controllers = data[M64_OFFSETS::num_controllers];
      read_int32(&data[M64_OFFSETS::num_input_frames], metadata._num_input_frames);
      std::cerr << "Input frame len is: " << metadata._num_input_frames << "\n";
      // perform type-punning pointer casts, since underlying types are defined
      read_int16(&data[M64_OFFSETS::start_type], *reinterpret_cast<uint16_t*>(&metadata.start_type));
      read_int32(&data[M64_OFFSETS::controllers], *reinterpret_cast<uint32_t*>(&metadata.controllers));
      
      read_str(&data[M64_OFFSETS::rom_name], metadata.rom_name);
      read_int32(&data[M64_OFFSETS::crc], metadata.crc);
      read_int16(&data[M64_OFFSETS::country_code], metadata.country_code);
      
      read_str(&data[M64_OFFSETS::video_plugin], metadata.video_plugin);
      read_str(&data[M64_OFFSETS::sound_plugin], metadata.sound_plugin);
      read_str(&data[M64_OFFSETS::input_plugin], metadata.input_plugin);
      read_str(&data[M64_OFFSETS::rsp_plugin], metadata.rsp_plugin);
      
      read_str(&data[M64_OFFSETS::authors], metadata.authors);
      read_str(&data[M64_OFFSETS::description], metadata.description);
    }
    
    m_inputs = std::vector<frame>(metadata.num_input_frames());
    unique_ptr<char[]> data = unique_ptr<char[]>(new char[metadata._num_input_frames * 4]);
    // seek to inputs at 0x0100
    in.seekg(M64_OFFSETS::start_of_data, ios::beg);
    in.read(data.get(), metadata._num_input_frames * 4);
    // x64 is little-endian, so everything lines up
    for (size_t i = 0; i < m_inputs.size(); i++) {
      size_t off = i * 4;
      m_inputs[i].buttons = static_cast<frame::button>((uint8_t(data[off]) << 8) | uint8_t(data[off + 1]));
      m_inputs[i].stick_x = data[off + 2];
      m_inputs[i].stick_y = data[off + 3];
    }
  }

  frame& m64::operator[](uint32_t frame) { return m_inputs[frame]; }
  const frame& m64::operator[](uint32_t frame) const { return m_inputs[frame]; }

  frame& m64::at(uint32_t frame) { return m_inputs.at(frame); }
  const frame& m64::at(uint32_t frame) const { return m_inputs.at(frame); }

  frame& m64::front() { return m_inputs.front(); }
  const frame& m64::front() const { return m_inputs.front(); }

  frame& m64::back() { return m_inputs.back(); }
  const frame& m64::back() const { return m_inputs.back(); }

  uint32_t m64::size() const { return metadata._num_input_frames; }

  m64::iterator m64::begin() { return m_inputs.begin(); }

  m64::iterator m64::end() { return m_inputs.end(); }

  m64::const_iterator m64::begin() const { return m_inputs.begin(); }

  m64::const_iterator m64::end() const { return m_inputs.end(); }

  m64::const_iterator m64::cbegin() const { return m_inputs.cbegin(); }

  m64::const_iterator m64::cend() const { return m_inputs.cend(); }

  void m64::push_back(const frame& frame) {
    if (metadata._num_input_frames == std::numeric_limits<uint32_t>::max()) {
      throw out_of_range("An .m64 cannot be longer than 2^32 - 1 frames");
    }
    m_inputs.push_back(frame);
    metadata._num_input_frames = m_inputs.size();
  }

  void m64::push_back(frame&& frame) {
    if (m_inputs.size() == std::numeric_limits<uint32_t>::max()) {
      throw out_of_range("An .m64 cannot be longer than 2^32 - 1 frames");
    }
    m_inputs.push_back(frame);
    metadata._num_input_frames = m_inputs.size();
  }

  void m64::pop_back() {
    if (m_inputs.size() > 0) {
      m_inputs.pop_back();
      metadata._num_input_frames = m_inputs.size();
    } else {
      throw out_of_range("Tried to pop element from empty M64");
    }
  }

  void frame::apply(pancake::sm64& game) const {
    game.get<uint16_t>("gControllerPads[0].button") =
      static_cast<uint16_t>(this->buttons);
    game.get<int8_t>("gControllerPads[0].stick_x") = this->stick_x;
    game.get<int8_t>("gControllerPads[0].stick_y") = this->stick_y;
  }

  void m64::dump(fs::path path) {
    auto dump_int32 = [](uint32_t x, char* p) {
      // Narrowing conversion takes the lowest byte
      p[0] = x;
      x >>= 8;
      p[1] = x;
      x >>= 8;
      p[2] = x;
      x >>= 8;
      p[3] = x;
    };
    auto dump_int16 = [](uint16_t x, char* p) {
      // Narrowing conversion takes the lowest byte
      p[0] = x;
      x >>= 8;
      p[1] = x;
    };
    auto dump_str = [](string x, char* p, unsigned bytes) {
      // Truncate if needed
      if (x.length() + 1 > bytes)
        x.resize(bytes - 1);
      for (size_t i = 0; i < x.length(); i++) {
        p[i] = x[i];
      }
      p[x.length()] = '\0';
    };

    std::ofstream out         = std::ofstream(path, ios::out | ios::binary);
    unique_ptr<char[]> buffer = unique_ptr<char[]>(new char[1024]);
    // Metadata
    std::copy(M64_SIG.begin(), M64_SIG.end(), &buffer[M64_OFFSETS::signature]);

    dump_int32(metadata.version, &buffer[M64_OFFSETS::version]);
    dump_int32(metadata.timestamp, &buffer[M64_OFFSETS::timestamp]);
    dump_int32(metadata.num_vis, &buffer[M64_OFFSETS::num_vis]);
    dump_int32(metadata.rerecords, &buffer[M64_OFFSETS::rerecords]);
    buffer[M64_OFFSETS::vis_per_s]       = metadata.vis_per_s;
    buffer[M64_OFFSETS::num_controllers] = metadata.num_controllers;
    dump_int32(metadata.num_input_frames(), &buffer[M64_OFFSETS::num_input_frames]);
    
    dump_int16(
      static_cast<uint16_t>(metadata.start_type), &buffer[M64_OFFSETS::start_type]);
    dump_int32(static_cast<uint32_t>(metadata.controllers), &buffer[M64_OFFSETS::controllers]);
    
    dump_str(metadata.rom_name, &buffer[M64_OFFSETS::rom_name], 32);
    dump_int32(metadata.crc, &buffer[M64_OFFSETS::crc]);
    dump_int32(metadata.country_code, &buffer[M64_OFFSETS::country_code]);
    
    dump_str(metadata.video_plugin, &buffer[M64_OFFSETS::video_plugin], 64);
    dump_str(metadata.sound_plugin, &buffer[M64_OFFSETS::sound_plugin], 64);
    dump_str(metadata.input_plugin, &buffer[M64_OFFSETS::input_plugin], 64);
    dump_str(metadata.rsp_plugin, &buffer[M64_OFFSETS::rsp_plugin], 64);
    
    dump_str(metadata.authors, &buffer[M64_OFFSETS::authors], 222);
    dump_str(metadata.description, &buffer[M64_OFFSETS::description], 256);

    out.seekp(0, ios::beg);
    out.write(&buffer[0], 1024);
    
    // Inputs
    size_t input_size = m_inputs.size() * 4;
    buffer.reset(new char[input_size]);
    for (size_t i = 0, j = 0; i < m_inputs.size(); j = (++i * 4)) {
      buffer[j]     = static_cast<uint16_t>(m_inputs[i].buttons);
      buffer[j + 1] = static_cast<uint16_t>(m_inputs[i].buttons) >> 8;
      buffer[j + 2] = m_inputs[i].stick_x;
      buffer[j + 3] = m_inputs[i].stick_y;
    }
    out.seekp(M64_OFFSETS::start_of_data, ios::beg);
    out.write(&buffer[0], input_size);
  }
}  // namespace pancake