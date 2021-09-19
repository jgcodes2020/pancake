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

#include <pancake/dwarf/type_lookup.hpp>
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
    struct impl;
    std::shared_ptr<impl> pimpl;

    void* const _impl_get(
      const std::string& expr,
      dwarf::base_type_info type = dwarf::base_type_info {
        dwarf::encoding::none, 0});

  public:
    class savestate final {
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
    void* get_unsafe(std::string expr) { return _impl_get(expr); }

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
    [[nodiscard]] savestate alloc_svst() const;
    /**
     * @brief Saves the game's state to the savestate buffer.
     *
     * @param st the savestate to save
     * @exception std::domain_error if the savestate is not linked to THIS game.
     */
    void save_svst(savestate& st) const;
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
    const std::variant<double, int64_t, nullptr_t> constant(
      std::string name) const;
  };
}  // namespace pancake
#endif