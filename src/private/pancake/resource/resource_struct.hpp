/**
 * @file resource_struct.hpp
 * @author jgcodes2020
 * @brief typedefs arrays as resources.
 * @version 0.1
 * @date 2021-06-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
 #ifndef _PANCAKE_RESOURCE_STRUCT_HPP_
 #define _PANCAKE_RESOURCE_STRUCT_HPP_
 #include <stdint.h>
 #include <array>
 
 namespace pancake::resources {
   template<size_t size>
   using resource = const std::array<uint8_t, size>;
 }
 #endif