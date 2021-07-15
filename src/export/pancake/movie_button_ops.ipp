#ifndef _PANCAKE_MOVIE_BUTTON_OPS_IPP_
#define _PANCAKE_MOVIE_BUTTON_OPS_IPP_
namespace pancake {
  m64::button operator|(m64::button lhs, m64::button rhs) {
    using impltype = std::underlying_type_t<m64::button>;
    using enumtype = m64::button;
    
    return static_cast<enumtype>(static_cast<impltype>(lhs) | static_cast<impltype>(rhs));
  }
  
  m64::button operator&(m64::button lhs, m64::button rhs) {
    using impltype = std::underlying_type_t<m64::button>;
    using enumtype = m64::button;
    
    return static_cast<enumtype>(static_cast<impltype>(lhs) & static_cast<impltype>(rhs));
  }
  
  m64::button operator^(m64::button lhs, m64::button rhs) {
    using impltype = std::underlying_type_t<m64::button>;
    using enumtype = m64::button;
    
    return static_cast<enumtype>(static_cast<impltype>(lhs) ^ static_cast<impltype>(rhs));
  }
  
  m64::button operator~(m64::button param) {
    using impltype = std::underlying_type_t<m64::button>;
    using enumtype = m64::button;
    
    return static_cast<enumtype>(~static_cast<impltype>(param));
  }
  
  m64::button& operator|=(m64::button& lhs, m64::button rhs) {
    return (lhs = lhs | rhs);
  }
  
  m64::button& operator&=(m64::button& lhs, m64::button rhs) {
    return (lhs = lhs & rhs);
  }
  
  m64::button& operator^=(m64::button& lhs, m64::button rhs) {
    return (lhs = lhs ^ rhs);
  }
}
#endif