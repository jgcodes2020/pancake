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

#include "pancake/exception.hpp"
#include "pancake/movie.hpp"

namespace pancake {
  
  /**
   * @brief An instance of the SM64 DLL.
   * 
   */
  class sm64 {
    friend struct m64::frame;
  private:
    struct impl;
    std::shared_ptr<impl> pimpl;
    
    // Internal pointer cache for other internal classes.
    std::unordered_map<std::string, void*> ptr_cache;
    
    void* const _impl_get(std::string expr);
    std::any _impl_constant(std::string name);
  public:
    class savestate {
      friend class sm64;
    private:
      struct impl;
      std::unique_ptr<impl> pimpl;
      
      void save(sm64 const& impl);
      void load(sm64& impl) const;
      
      savestate(sm64 const& game);
    };
    /**
     * @brief Loads libsm64.
     * 
     * @param path path to libsm64
     */
    sm64(std::filesystem::path path);
    
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
     * @note This method is inherently unsafe as you CAN cast to the wrong type and ruin your code.
     * 
     * @tparam T Must be an integer or floating-point type that is not `long double`
     * @param expr an accessor expression.
     * @return T& the value from the accessor expression
     * @exception std::domain_error if the resulting field is not a fundamental type
     */
    template<typename T>
    [[nodiscard]] T& get(std::string expr) {
      static_assert(std::disjunction_v<
        std::conjunction<
          std::is_arithmetic<T>,
          std::negation<std::is_same<T, long double>>
        >,
        std::is_same<T, void*>
      >, "T should be any integer, or float or double or void pointer");
      
      // unsafe cast back to correct type
      return *reinterpret_cast<T*>(_impl_get(expr));
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
    void load_svst(savestate const& st);
    
    /**
     * @brief Loads a constant. 
     * 
     * @tparam T either double, uint64_t, or nullptr_t
     * @param name the name of the constant
     * @return const T the constant
     */
    template<typename T>
    const T constant(std::string name) {
      static_assert(std::disjunction<
        std::is_null_pointer<T>,
        std::is_same<T, double>,
        std::is_same<T, int64_t>
      >::value, "T must be either double, int64_t, or nullptr_t");
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