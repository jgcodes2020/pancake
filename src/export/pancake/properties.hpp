/**
 * @file movie.hpp
 * @author jgcodes2020
 * @brief Contains a simple computed-properties framework.
 * @version 0.1
 * @date 2021-05-19
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef __pancake_PROPERTIES_HPP
#define __pancake_PROPERTIES_HPP

#include <functional>

namespace pancake {
  using std::function;

  /**
   * @brief A simple computed property that provides syntactic sugar over setters/getters.
   * @tparam T the type of this property
   */
  template <class T>
  class property {
  private:
    function<T()> m_getter;
    function<void(T)> m_setter;
  public:
    /**
     * @brief Construct a new property object
     * 
     * @tparam GET A type assignable to `std::function<T()>`.
     * @tparam SET A type assignable to `std::function<T&(T)>`.
     * @param getter A function which gets this property.
     * @param setter A function which sets this property.
     */
    template<typename GET, typename SET>
    property(GET getter, SET setter) {
      m_getter = getter;
      m_setter = setter;
    }

    //Properties are non-copiable and non-movable
    property(const property&) = delete;
    property(property&&) = delete;

    /**
     * @brief Gets this property.
     * 
     * @return the value of this property
     */
    operator T() const {
      return m_getter();
    }
    /**
     * @brief Sets this property, as if by the standard assignment operator.
     * 
     * @param value the value to assign
     */
    void operator=(const T& value){
      m_setter(value);
    }
    
    /**
     * @brief Gets this property.
     * 
     * @return the value of this property
     */
    T get() {
      return m_getter();
    }
    /**
     * @brief Sets this property.
     * 
     * @param value the value to assign
     */
    void set(T& value) {
      m_setter(value);
    }
  };
}
#endif