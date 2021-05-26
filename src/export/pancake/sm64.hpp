/**
 * @file sm64.hpp
 * @author jgcodes2020
 * @brief Contains utilities to load and access the pancake DLL by bad_boot
 * @version 0.1
 * @date 2021-05-18
 * 
 * @copyright Copyright (c) 2021
 * 
 */

//Include required library headers
#include "pancake/movie.hpp"
#include <algorithm>

#ifndef __PANCAKE_SM64_HPP
#define __PANCAKE_SM64_HPP

#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <array>
#include <vector>
#include <stdexcept>
#include <memory>

using std::string, std::array, std::vector;
using std::ios, std::fstream, std::ostringstream;

namespace pancake {
  /**
   * @brief Indicates an error to do with shared libraries.
   * Provides a field for a platform-specific error codes.
   */
  class shared_lib_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
  public:
    uint64_t error_code;
  };
  /**
   * @brief Represents a savestate.
   */
  class savestate {
  private:
    struct impl;
    std::unique_ptr<savestate::impl> p_impl;
  public:
    /**
     * @brief Retrieves a buffer from this savestate.
     *
     * @param i the index of the element to retrieve
     * @return the buffer
     */
    vector<uint8_t>& operator[](uint64_t i);
  };
  /**
   * @brief Represents the sm64 shared library and
   * provides a convenient interface for interacting with it.
   */
  class sm64 {
  private:
    struct impl;
    std::unique_ptr<sm64::impl> p_impl;
    /**
     * @brief Locates a symbol in the shared library.
     *
     * @param var a symbol in the shared library
     * @return a pointer to the requested symbol.
     */
    void* _impl_locate(string var);
  public:
    /**
     * @brief Loads libsm64.
     *
     * @param path Path to libsm64
     */
    sm64(string path);
    ~sm64();

    
    template<typename T = void>
    T* locate(string var) {
      return reinterpret_cast<T*>(_impl_locate(var));
    }
    /**
     * @brief Steps 1 frame forward.
     * @note It is the developer's responsibility to call sm64::set_input() beforehand.
     */
    void advance();
    /**
     * @brief Allocates a savestate buffer, based on
     * the version info provided at construction time.
     *
     * @return a savestate object, which starts out empty.
     */
    savestate allocate_slot();
    /**
     * @brief Saves libsm64's current state to a savestate.
     *
     * @param save the savestate to save to
     */
    void save_slot(savestate& save);
    /**
     * @brief Loads a savestate into libsm64's current state.
     *
     * @param save the savestate to load from
     */
    void load_slot(savestate& save);

    void set_input(uint16_t buttons, int8_t stick_x, int8_t stick_y);
    #ifdef __pancake_MOVIE_HPP
    /**
     * @brief Sets the current input. You will need to include "pancake/movie.hpp" to use this function.
     * 
     * @param input an input_frame with the needed input
     */
    void set_input(input_frame input) {
      set_input(input.m_buttons, input.m_stick_x, input.m_stick_y);
    }
    #endif
  };
}

#endif
