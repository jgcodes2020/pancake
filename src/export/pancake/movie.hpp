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
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <string>
#include <filesystem>
#include <iostream>



namespace pancake {
  class sm64;
  
  class m64 {
  public:
    /**
     * @brief Enum representing button inputs.
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
      C_RIGHT = 0x0001
    };
    struct frame final {
      
      button buttons;
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
    uint32_t m_inputs_len;
  public:
    using iterator = decltype(m_inputs)::iterator;
    using const_iterator = decltype(m_inputs)::const_iterator;
    
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
     * @brief Copies the inputs from the specified iterators.
     * 
     * @tparam input_it an input iterator of m64::frame
     * @param begin the beginning iterator
     * @param end the ending iterator
     */
    template<typename input_it>
    m64(input_it begin, input_it end) :
      m_inputs(begin, end) {
      size_t rsize = m_inputs.size();
      // if any of the upper 32 bits are set, it's too big
      if (rsize & 0xFFFFFFFF00000000) {
        throw std::domain_error("M64 files are limited to 2^32 - 1 inputs");
      }
      m_inputs_len = rsize;
    }
    
    /**
     * @brief Returns a specific input frame.
     * 
     * @param frame the index of the frame
     * @return the input frame at the specified index
     */
    frame& operator[](uint32_t frame);
    
    /**
     * @brief Returns the first frame.
     * 
     * @return the first frame
     */
    frame& front();
    
    /**
     * @brief Returns the last frame.
     * 
     * @return the last frame
     */
    frame& back();
    
    /**
     * @brief Returns the length of this .m64.
     * 
     * @return uint32_t the length of this .m64.
     */
    uint32_t size() const;
    
    /**
     * @brief Returns an iterator to the beginning of the input frames.
     * 
     * @return auto an iterator to the beginning of the input frames
     */
    iterator begin();
    
    /**
     * @brief Returns an iterator to the end of the input frames.
     * 
     * @return auto an iterator to the end of the input frame
     */
    iterator end();
    
    /**
     * @brief Returns a const iterator to the beginning of the input frames.
     * 
     * @return auto a const iterator to the beginning of the input frames
     */
    const_iterator begin() const;
    
    /**
     * @brief Returns a const iterator to the end of the input frames.
     * 
     * @return auto a const iterator to the end of the input frames
     */
    const_iterator end() const;
    
    /**
     * @brief Returns a const iterator to the beginning of the input frames.
     * 
     * @return auto a const iterator to the beginning of the input frames
     */
    const_iterator cbegin() const;
    
    /**
     * @brief Returns a const iterator to the end of the input frames.
     * 
     * @return auto a const iterator to the end of the input frames
     */
    const_iterator cend() const;
    
    /**
     * @brief Adds a copy of this frame to the end of the inputs.
     * 
     * @param frame the frame to add
     */
    void push_back(const frame& frame);
    
    /**
     * @brief Adds a frame to the end of the inputs.
     * 
     * @param frame the frame to add
     */
    void push_back(frame&& frame);
    
    /**
     * @brief Removes the last frame.
     */
    void pop_back();
  };
  
  m64::button operator|(m64::button lhs, m64::button rhs);
  m64::button operator&(m64::button lhs, m64::button rhs);
  m64::button operator^(m64::button lhs, m64::button rhs);
  m64::button operator~(m64::button param);
  
  m64::button& operator|=(m64::button& lhs, m64::button rhs);
  m64::button& operator&=(m64::button& lhs, m64::button rhs);
  m64::button& operator^=(m64::button& lhs, m64::button rhs);
}
#include <pancake/movie_button_ops.ipp>
#endif