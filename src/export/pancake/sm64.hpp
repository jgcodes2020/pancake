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

#include <any>
#include <stdexcept>
#include <string>
#include <sstream>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vcruntime_typeinfo.h>

namespace pancake {
  
  namespace fs = std::filesystem;
  
  /**
   * @brief Runtime wrapper around a libsm64 struct.
   */
  class struct_t {
  private:
    struct impl;
    std::unique_ptr<impl> pimpl;
    
    std::pair<type_info&, void*> get_member(std::string name);
    struct_t();
  public:
    const std::string type_name;
    
    /**
     * @brief Gets a member. Uses RTTI to ensure type safety when retrieving values.
     * 
     * @tparam T must match the type of the requested member.
     * @param name the name of the member
     * @return A reference to the specified member
     */
    template<class T>
    T& operator[](std::string name) {
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
    friend class struct_t;
  private:
    struct impl;
    std::shared_ptr<impl> pimpl;
    
    std::pair<type_info&, void*> _impl_get(std::string expr);
    std::any _impl_constant(std::string name);
  public:
    class savestate {
    private:
      struct impl;
      std::unique_ptr<impl> pimpl;
      
      void save(std::shared_ptr<sm64::impl> impl);
      void load(std::shared_ptr<sm64::impl> impl);
    };
    /**
     * @brief Loads libsm64.
     * 
     * @param path path to libsm64
     */
    sm64(fs::path path);
    
    /**
     * @brief Loads libsm64.
     * 
     * @param path path to libsm64
     */
    sm64(std::string path);
    
    /**
     * @brief Loads libsm64.
     * 
     * @param path path to libsm64
     */
    sm64(const char* path);
    
    /**
     * @brief Returns a pointer to a specific field.
     * 
     * @tparam T Must be an integer or floating-point type that is not `long double`
     * @param expr an accessor expression.
     * @return T& the value from the accessor expression
     */
    template<typename T>
    T& get(std::string expr) {
      static_assert(std::conjunction<
        std::is_arithmetic<T>,
        std::negation<std::is_same<T, long double>>
      >::value, "T should be any integer, or float or double");
      
      auto impl = _impl_get(expr);
      // type-safety check
      if (typeid(T) != impl.first) {
        std::stringstream fmt;
        fmt << "Expected T to be " << impl.first.name() << ", instead was " << typeid(T).name();
        throw type_error(fmt.str());
      }
      // unsafe cast back to correct type
      return *reinterpret_cast<T*>(impl.second);
    }
    
    /**
     * @brief Advances the savestate forward by 1 frame.
     * 
     */
    void advance();
    
    /**
     * @brief Allocates a savestate buffer.
     * 
     * @return a new savestate bound to this game.
     */
    savestate alloc_svst();
    /**
     * @brief Saves the game's state to the savestate buffer.
     * 
     * @param st the savestate to save
     * @exception std::domain_error if the savestate is not linked to THIS game.
     */
    void save_svst(savestate& st);
    /**
     * @brief Loads the game's state from a savestate buffer.
     * 
     * @param st 
     * @exception std::domain_error if the savestate is not linked to THIS game.
     */
    void load_svst(const savestate& st);
    
    /**
     * @brief Loads a constant from 
     * 
     * @tparam T 
     * @param name 
     * @return const T 
     */
    template<typename T>
    const T constant(std::string name) {
      static_assert(std::disjunction<
        std::is_void<T>,
        std::is_same<T, double>,
        std::is_same<T, uint64_t>
      >::value, "T must be either double, uint64_t, or void");
      std::any value = _impl_constant(name);
      if (typeid(T) != value.type()) {
        std::stringstream fmt;
        fmt << "Expected T to be " << value.type().name() << ", instead was " << typeid(T).name();
        throw type_error(fmt.str());
      }
      return std::any_cast<T>(value);
    }
  };
}
#endif