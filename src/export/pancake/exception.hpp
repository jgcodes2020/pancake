/**
 * @file exception.hpp
 * @author jgcodes2020
 * @brief Provides Pancake exception classes.
 * @version 0.1
 * @date 2021-06-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */
 #ifndef _PANCAKE_EXCEPTION_HPP_
 #define _PANCAKE_EXCEPTION_HPP_
 #include <stdexcept>
 
 namespace pancake {
   /**
    * @brief Specifies that an accessor expression is not complete.
    */
   class incomplete_accessor : public std::domain_error {
     using std::domain_error::domain_error;
   };
   /**
    * @brief Specifies that a type mismatch was detected at runtime.
    */
   class type_error : public std::logic_error {
     using std::logic_error::logic_error;
   };
 }
 #endif