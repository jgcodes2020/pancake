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
#include <cstddef>
#include <filesystem>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

#include "pancake/dl/pdl.hpp"
#include "pancake/dwarf/types.hpp"
#include <pancake/dwarf/type_info.hpp>
#include <pancake/exception.hpp>
#include <pancake/movie.hpp>

using std::nullptr_t;

namespace pancake {
  
  

  /**
   * @brief An instance of the SM64 DLL.
   *
   */
  class sm64 {
    friend struct frame;

  private:
    struct expr_info {
      void* ptr;
      pancake::dwarf::base_type_info type;
    };
    
    dl::library lib;
    dwarf::debug dbg;
    std::unordered_map<std::string, expr_info> cache;

    void* _impl_get(
      const std::string& expr,
      dwarf::base_type_info type = dwarf::base_type_info {
        dwarf::encoding::none, 0});

  public:
    class savestate final {
      friend class sm64;

    private:
      struct impl;
      std::unique_ptr<impl> p_impl;
      
      savestate(sm64 const& game);
    public:
      void save();
      void load() const;
    };
    /**
     * @brief Loads libsm64.
     *
     * @param path path to libsm64
     */
    sm64(const std::filesystem::path& path);

    /**
     * @brief Returns a reference to a specific field.
     * @note This method does type checking. You can use `get_unsafe()` if you
     * favour performance over safety.
     *
     * @tparam T Must be an integer or floating-point type that is not `long
     * double`
     * @param expr an accessor expression.
     * @return T& the value from the accessor expression
     * @exception std::domain_error if the resulting field is not a fundamental
     * type
     * @exception pancake::type_error if the resulting field does not match `T`
     */
    template <typename T>
    [[nodiscard]] T& get(std::string expr) {
      static_assert(
        std::is_arithmetic_v<T> && !std::is_same_v<T, long double>,
        "T should be any integer, or float or double or void pointer");

      // unsafe cast back to correct type
      return *reinterpret_cast<T*>(_impl_get(expr, dwarf::get_type_info<T>()));
    }

    /**
     * @brief Returns a pointer to a specific field.
     * @note This method does not do type checking. You can use
     * `template<typename T> get()` if you favour safety over performance.
     *
     * @param expr an accessor expression.
     * @return void* a pointer to the value from the accessor expression
     * @exception std::domain_error if the resulting field is not a fundamental
     * type
     */
    void* get_unsafe(const std::string& expr) { return _impl_get(expr); }
    
    template<typename T>
    decltype(auto) get_symbol(const std::string& name) {
      return lib.get_symbol<T>(name);
    }

    /**
     * @brief Advances the game forward by 1 frame.
     *
     */
    void advance();

    /**
     * @brief Allocates a savestate buffer.
     *
     * @return a new savestate bound to this game.
     */
    [[nodiscard]] savestate alloc_svst() const;

    /**
     * @brief Loads a constant.
     *
     * @tparam T either double, uint64_t, or nullptr_t
     * @param name the name of the constant
     * @return const T the constant
     */
    const std::variant<double, int64_t, nullptr_t> constant(
      std::string name) const;
    
    /**
     * @brief Returns the library instance associated with this sm64.
     * 
     * @return dl::library the library instance
     */
    dl::library& get_lib();
    /**
     * @brief Returns the debug info associated with this sm64.
     * 
     * @return dwarf::debug the debug info
     */
    dwarf::debug& get_debug_info();
  };
}  // namespace pancake
#endif