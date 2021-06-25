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

#include <stdexcept>
#include <string>
#include <sstream>
#include <filesystem>
#include <memory>
#include <any>
#include <unordered_map>

namespace pancake {
  using std::unique_ptr, std::any;
  using std::string;
  
  namespace fs = std::filesystem;
  
  /**
   * @brief Runtime wrapper around a libsm64 struct.
   */
  class struct_t {
  private:
    struct impl;
    unique_ptr<impl> pimpl;
    
    std::pair<type_info, void*> get_member(string name);
  public:
    const string type_name;
    
    /**
     * @brief Gets a member. Uses RTTI to ensure type safety when retrieving values.
     * 
     * @tparam T must match the type of the requested member.
     * @param name the name of the member
     * @return A reference to the specified member
     */
    template<class T>
    T& get(string name) {
      auto member = get_member(name);
      if (typeid(T) != member.first) {
        std::stringstream fmt;
        fmt << "Member " << name << " is of type ";
        fmt << member.first.name() << ", not of type " << typeid(T).name();
        throw std::invalid_argument(fmt.str());
      }
      return *reinterpret_cast<T*>(member.second);
    }
  };
  
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