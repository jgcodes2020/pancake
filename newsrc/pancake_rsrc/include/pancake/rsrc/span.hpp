#ifndef _PANCAKE_RSRC_SPAN_
#define _PANCAKE_RSRC_SPAN_

#if defined(__cpp_lib_span)
#include <span>
#else
#include <cstddef>
#include <gsl/span>
namespace std {
  template<typename _T, size_t extent = gsl::dynamic_extent>
  using span = gsl::span<_T, extent>;
}
#endif

#endif