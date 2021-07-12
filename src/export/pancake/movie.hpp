/**
 * @file movie.hpp Handling for Mupen64 recordings
 * @author jgcodes2020
 * @brief 
 * @version 0.1
 * @date 2021-06-28
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_MOVIE_HPP_
#define _PANCAKE_MOVIE_HPP_

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

namespace pancake {
  class sm64;
  class m64 {
  public:
    struct frame final {
      /**
       * @brief Unscoped enum representing button inputs.
       * 
       */
      class button final {
        enum : uint16_t {
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
          C_RIGHT = 0x0001
        };
        button() = delete;
      };
      
      uint16_t buttons;
      int8_t stick_x, stick_y;
      
      /**
       * @brief Sets the input on an `sm64` instance to the current input.
       * 
       * @param game 
       */
      void apply(sm64& game) const;
    };
  private:
    std::vector<frame> m_inputs;
    uint32_t m_len;
  public:
    
    /**
     * @brief Loads an .m64 file from a path.
     * 
     * @param path the path to load from
     */
    m64(std::string path);
    /**
     * @brief Loads an .m64 from a path.
     * 
     * @param path  the path to load from
     */
    m64(const char* path);
    /**
     * @brief Loads an .m64 file from a path.
     * 
     * @param path the path to load from
     */
    m64(std::filesystem::path path);
    
    /**
     * @brief Returns a specific input frame.
     * 
     * @param frame the index of the frame
     * @return the input frame at the specified index
     */
    frame& operator[](uint32_t frame);
    
    /**
     * @brief Returns the length of this .m64.
     * 
     * @return uint32_t the length of this .m64.
     */
    uint32_t size() const;
    
    /**
     * @brief Returns an iterator to the beginning of the input frames.
     * 
     * @return auto the max number of input frames.
     */
    auto begin() const;
    
    /**
     * @brief Returns an iterator to the end of the input frames.
     * 
     * @return auto the max number of input frames.
     */
    auto end() const;
  };
}
#endif