.. _api_exception:

exception.hpp
==============
Exceptions that are specific to Pancake and deserve their own class.

.. cpp:namespace:: pancake
.. cpp:class:: null_pointer_exception : public std::logic_error
  This error is internal. It indicates that something attempted to operate
  on a null pointer.
  
.. cpp:class:: type_error : public std::logic_error
  This error indicates that a specified type did not match one extracted from
  libsm64.
  
.. cpp:class:: incomplete_accessor : public std::invalid_argument
  This error indicates that an accessor expression did not refer to a valid value.
  It will likely be removed when I figure out struct/union support.
  