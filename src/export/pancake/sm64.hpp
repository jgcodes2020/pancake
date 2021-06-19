/**
 * @file sm64.hpp
 * @author jgcodes2020
 * @brief Interface to libsm64
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 */
#ifndef _PANCAKE_SM64_HPP_
#define _PANCAKE_SM64_HPP_

#include <string>
#include <filesystem>
#include <memory>
#include <any>
#include <unordered_map>

namespace pancake {
  using std::unique_ptr, std::any;
  using std::string;
  
  namespace fs = std::filesystem;
  
  /**
   * @brief A map of strings to pointers to their respective struct fields.
   */
  using struct_t = std::unordered_map<string, any>;
  class sm64 {
  private:
    struct impl;
    unique_ptr<impl> pimpl;
  public:
    /**
     * @brief Loads libsm64.
     * 
     * @param path path to libsm64
     */
    sm64(fs::path path);
    
    /**
     * @brief Returns the value of a variable.
     * @details This allows `.`, `->`, `[]`. If you need specific bits, wrap in a bitset.
     * 
     * @param expr a C-style accessor expression.
     * @return a std::any. If nothing was found, returns nullptr. Structs are replaced with `struct_t`.
     */
    any read(string expr);
    
    /**
     * @brief Returns the value of a constant.
     * @details Said constant is usually a macro.
     * 
     * @param expr 
     * @return const any 
     */
    const any constant(string expr);
    
    /**
     * @brief Sets the value of a variable.
     * 
     * @tparam T 
     * @param value 
     */
    template<typename T>
    void write(T value);
  };
}
#endif