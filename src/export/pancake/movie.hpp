/**
 * @file movie.hpp
 * @author jgcodes2020
 * @brief Contains utilities for the reading of Mupen64 input files (manipulation to come)
 * @version 0.1
 * @date 2021-05-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __pancake_MOVIE_HPP
#define __pancake_MOVIE_HPP

#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::ios, std::fstream;
using std::string, std::vector;
using std::ostringstream;
using std::out_of_range;

namespace pancake { 
  /**
   * @brief Represents all input data for 1 frame.
   */
  struct input_frame {
  public:
    /**
     * @brief A bunch of handy constants for buttons.
     */
    enum class button : uint16_t {
      /**
       * @brief The A button.
       */
      A = 0x8000,
      /**
       * @brief The B button.
       */
      B = 0x4000,
      /**
       * @brief The Z trigger.
       */
      Z = 0x2000,

      /**
       * @brief The START button.
       */
      START = 0x1000,

      /**
       * @brief The UP key on the D-pad.
       */
      D_UP = 0x0800,
      /**
       * @brief The DOWN key on the D-pad.
       */
      D_DOWN = 0x0400,
      /**
       * @brief The LEFT key on the D-pad.
       */
      D_LEFT = 0x0200,
      /**
       * @brief The RIGHT key on the D-pad.
       */
      D_RIGHT = 0x0100,

      /**
       * @brief The L trigger.
       */
      L = 0x0020,
      /**
       * @brief The R trigger.
       */
      R = 0x0010,

      /**
       * @brief The C-up (aka C^) button.
       */
      C_UP = 0x0008,
      /**
       * @brief The C-down (aka Cv) button.
       */
      C_DOWN = 0x0004,
      /**
       * @brief The C-left (aka C< or C) button.
       */
      C_LEFT = 0x0002,
      /**
       * @brief The C-right (aka C>) button.
       */
      C_RIGHT = 0x0001,
    };
    /**
     * @brief A bitmask for the buttons. Each bit is tied to a button on the controller.
     */
    uint16_t m_buttons;
    /**
     * @brief Represents the joystick coordinates. m_stick_x gives the x-coordinate and m_stick_y
     * gives the y-coordinate.
     */
    int8_t m_stick_x, m_stick_y;

    /**
     * @brief Creates an input frame using a button bitmask and stick coordinates.
     * 
     * @param buttons a bitmask containing the buttons to push down
     * @param stick_x the joystick's x-coordinate
     * @param stick_y the joystick's y-coordinate 
     */
    input_frame(uint16_t buttons, int8_t stick_x, int8_t stick_y) :
      m_buttons(buttons), m_stick_x(stick_x), m_stick_y(stick_y) {}
    /**
     * @brief Creates an input frame using a list of buttons and stick coordinates.
     * 
     * @param buttons a list of buttons to push down.
     * @param stick_x the joystick's x-coordinate
     * @param stick_y the joystick's y-coordinate
     */
    input_frame(std::initializer_list<button> buttons, int8_t _stick_x, int8_t stick_y) :
      m_stick_x(_stick_x), m_stick_y(stick_y) {
      uint16_t btn_acc = 0;
      for (auto& btn : buttons) {
        btn_acc |= static_cast<uint16_t>(btn);
      }
      m_buttons = btn_acc;

    }
    
    /**
     * @brief Checks whether a button is pressed.
     * 
     * @param btn the button to check
     * @return true if the button is pressed, false otherwise
     */
    bool pressed(button btn) {
      return (m_buttons & static_cast<uint16_t>(btn));
    }
    /**
     * @brief Creates a new blank input frame (no buttons pressed, stick at (0, 0))
     * 
     * @return a blank input frame
     */
    static input_frame neutral() {
      return input_frame({}, 0, 0);
    }
  };

  /**
   * @brief Represents a .m64 input movie.
   * @details This class can load m64s. Editing is currently not added, I might do that later.
   */
  class m64_movie {
  private:
    vector<input_frame> m_frames;
  public:
    /**
     * @brief Loads a .m64 file from the specified file.
     *
     * @param path path to an m64 file
     */
    m64_movie(string path) {
      fstream file = fstream(path, ios::in | ios::binary);
      //seek to and read length
      file.seekg(0x018, ios::beg);
      char len_bytes[4];
      file.read(len_bytes, 4);

      const uint32_t frames =
        ((uint8_t) len_bytes[0]) |
        ((uint8_t) len_bytes[1] << 8) |
        ((uint8_t) len_bytes[2] << 16) |
        ((uint8_t) len_bytes[3] << 24);

      //seek to data, read exactly as many frames as read earlier
      file.seekg(0x400, ios::beg);
      m_frames.reserve(frames);
      {
        char* data = new char[4 * frames];
        file.read(&data[0], 4 * frames);

        for (size_t i = 0; i < frames; i++) {
          const size_t next_block = i * 4;
          const uint16_t buttons =
            ((uint8_t) data[next_block] << 8) |
            ((uint8_t) data[next_block + 1]);
          m_frames.push_back(input_frame {
            buttons, 
            ((int8_t) data[next_block + 2]), 
            ((int8_t) data[next_block + 3])
          });
        }
        delete[] data;
      }
    }

    /**
     * @brief Retrieves a frame of input from the movie.
     * 
     * @param i the index of the frame
     * @return the inputs corresponding to this frame
     */
    input_frame& operator[](size_t i) {
      if (i >= m_frames.size()) {
        ostringstream fmt;
        fmt << "Index " << i << " out of range for size " << m_frames.size();
        throw out_of_range(fmt.str());
      }
      return m_frames[i];
    }

    /**
     * @brief Returns the size of this movie.
     * 
     * @return the size of this movie.
     */
    size_t size() {
      return m_frames.size();
    }

    //TODO export m64
  };
} // namespace pancake

#endif