#ifndef _PANCAKE_RSRC_SPANSTREAM_
#define _PANCAKE_RSRC_SPANSTREAM_

#include <pancake/rsrc/span.hpp>
#if defined(__cpp_lib_spanstream)
  #include <spanstream>
#else
  #include <ios>
  #include <limits>
  #include <memory>
  #include <stdexcept>
  #include <streambuf>
  #include <string>
  #include <type_traits>
#endif

namespace std {
#if !defined(__cpp_lib_spanstream)
  template <typename _CharT, typename _Traits = char_traits<_CharT>>
  class basic_spanbuf : public basic_streambuf<_CharT, _Traits> {
  public:
    using char_type   = _CharT;
    using int_type    = typename _Traits::int_type;
    using pos_type    = typename _Traits::pos_type;
    using off_type    = typename _Traits::off_type;
    using traits_type = _Traits;

  private:
    using base_type = basic_streambuf<_CharT, _Traits>;
    using span_type = span<_CharT>;

    span<_CharT> m_data;
    ios_base::openmode m_mode;

  public:
    // Constructors
    // ============

    explicit basic_spanbuf(
      span_type _s,
      ios_base::openmode _flags = (ios_base::in | ios_base::out)) :
        base_type(), m_mode(_flags) {
      this->span(_s);
    }

    basic_spanbuf(base_type&& _rhs) :
        base_type(std::move(_rhs)),
        m_mode(std::move(_rhs.m_mode)),
        m_data(std::move(_rhs.m_data)) {}

    explicit basic_spanbuf(ios_base::openmode _flags) :
        basic_spanbuf(span_type(), _flags) {}
    basic_spanbuf() : basic_spanbuf(ios_base::in | ios_base::out) {}

    // Assignment and swapping
    // =======================

    basic_spanbuf& operator=(basic_spanbuf&& _rhs) {
      swap(_rhs);
      return *this;
    }

    void swap(basic_spanbuf&& _rhs) {
      base_type::swap(_rhs);
      std::swap(m_mode, _rhs.m_mode);
      std::swap(m_data, _rhs.m_data);
    }

    span_type span() const noexcept {
      if (m_mode & ios_base::out) {
        return span_type(this->pbase(), this->pptr());
      }
      else {
        return m_data;
      }
    }

    void span(span_type _s) noexcept {
      m_data = _s;

      if (m_mode & ios_base::out) {
        this->setp(_s.data(), _s.data() + _s.size());
        if (m_mode & ios_base::ate) {
          this->pbump(_s.size());
        }
      }
      if (m_mode & ios_base::in) {
        this->setg(_s.data(), _s.data(), _s.data() + _s.size());
      }
    }

  private:
    void set_pptr_off(off_type _off) {
      // Repeated conversion magic
      this->setp(this->pbase(), this->epptr());
      while (_off > std::numeric_limits<int>::max()) {
        this->pbump(std::numeric_limits<int>::max());
        _off -= std::numeric_limits<int>::max();
      }
      this->pbump(_off);
    }

  public:
    virtual pos_type seekoff(
      off_type _off, ios_base::seekdir _way, ios_base::openmode _flags) {
      bool is_input  = _flags & ios_base::in;
      bool is_output = _flags & ios_base::out;

      switch (_way) {
        case ios_base::cur: {
          if (is_input) {
            if (is_output) {
              throw std::invalid_argument(
                "seekdir cur requires a single openmode flag");
            }

            off_type baseoff = this->gptr() - this->eback();
            off_type newoff;

            if (__builtin_add_overflow(baseoff, _off, &newoff)) {
              throw std::overflow_error(
                "Next pointer would have overflowed during addition");
            }
            else if (newoff < 0 || newoff > m_data.size()) {
              throw std::out_of_range(
                "Next pointer would've been out of bounds");
            }
            else {
              this->setg(this->eback(), this->eback() + newoff, this->egptr());
            }
            return newoff;
          }
          else if (is_output) {
            off_type baseoff = this->pptr() - this->pbase();
            off_type newoff;

            if (__builtin_add_overflow(baseoff, _off, &newoff)) {
              throw std::overflow_error(
                "Next pointer would have overflowed during addition");
            }
            else if (newoff < 0 || newoff > m_data.size()) {
              throw std::out_of_range(
                "Next pointer would've been out of bounds");
            }
            else {
              set_pptr_off(newoff);
            }
            return newoff;
          }
        } break;
        case ios_base::beg: {
          if (_off < 0 || _off > m_data.size()) {
            throw std::out_of_range("Next pointer would've been out of bounds");
          }
          if (is_input) {
            this->setg(this->eback(), this->eback() + _off, this->egptr());
          }
          if (is_output) {
            set_pptr_off(_off);
          }
          return _off;
        } break;
        case ios_base::end: {
          if (
            _off > 0 ||
            _off < -static_cast<make_signed_t<size_t>>(m_data.size())) {
            throw std::out_of_range("Next pointer would've been out of bounds");
          }
          if (is_input) {
            this->setg(this->eback(), this->egptr() + _off, this->egptr());
          }
          if (is_output) {
            set_pptr_off((this->epptr() - this->pbase()) + _off);
          }
          return (this->epptr() - this->pbase()) + _off;
        } break;
        default: break;
      }
    }

    virtual pos_type seekpos(off_type _off, ios_base::openmode _flags) {
      return seekoff(_off, ios_base::beg, _flags);
    }
    virtual base_type setbuf(_CharT* s, size_t size) {
      this->span(std::span<_CharT*>(s, size));
      return *this;
    }
  };

  template <typename _CharT, typename _Traits>
  void swap(
    basic_spanbuf<_CharT, _Traits> _x, basic_spanbuf<_CharT, _Traits> _y) {
    _x.swap(_y);
  }

  template <typename _CharT, typename _Traits = char_traits<_CharT>>
  class basic_ispanstream : public basic_istream<_CharT, _Traits> {
  public:
    using char_type   = _CharT;
    using int_type    = typename _Traits::int_type;
    using pos_type    = typename _Traits::pos_type;
    using off_type    = typename _Traits::off_type;
    using traits_type = _Traits;

  private:
    using span_type   = span<_CharT>;
    using buf_type    = basic_spanbuf<_CharT, _Traits>;
    using stream_type = basic_istream<_CharT, _Traits>;

    buf_type m_buf;

  public:
    // Constructors
    // ============
    explicit basic_ispanstream(
      std::span<_CharT> _s, ios_base::openmode _flags = ios_base::in) :
        stream_type(&m_buf), m_buf(_s, _flags) {}

    basic_ispanstream(const basic_ispanstream&) = delete;

    basic_ispanstream(basic_ispanstream&& rhs) :
        stream_type(std::move(rhs.m_buf)), m_buf(std::move(rhs.m_buf)) {
      this->set_rdbuf(&m_buf);
    }

    // Member functions
    // ================
    void swap(basic_ispanstream& rhs) {
      stream_type::swap(rhs);
      m_buf.swap(rhs.m_buf);
    }

    buf_type* rdbuf() const noexcept {
      return const_cast<buf_type*>(addressof(m_buf));
    }

    span_type span() { return m_buf.span(); }

    void span(span_type s) { m_buf.span(s); }
  };

  template <typename _CharT, typename _Traits>
  void swap(
    basic_ispanstream<_CharT, _Traits> _x,
    basic_ispanstream<_CharT, _Traits> _y) {
    _x.swap(_y);
  }

  template <typename _CharT, typename _Traits = char_traits<_CharT>>
  class basic_ospanstream : public basic_ostream<_CharT, _Traits> {
  public:
    using char_type   = _CharT;
    using int_type    = typename _Traits::int_type;
    using pos_type    = typename _Traits::pos_type;
    using off_type    = typename _Traits::off_type;
    using traits_type = _Traits;

  private:
    using span_type   = span<_CharT>;
    using buf_type    = basic_spanbuf<_CharT, _Traits>;
    using stream_type = basic_ostream<_CharT, _Traits>;

    buf_type m_buf;

  public:
    // Constructors
    // ============
    explicit basic_ospanstream(
      std::span<_CharT> _s, ios_base::openmode _flags = ios_base::out) :
        stream_type(&m_buf), m_buf(_s, _flags) {}

    basic_ospanstream(const basic_ospanstream&) = delete;

    basic_ospanstream(basic_ospanstream&& rhs) :
        stream_type(std::move(rhs.m_buf)), m_buf(std::move(rhs.m_buf)) {
      this->set_rdbuf(&m_buf);
    }

    // Member functions
    // ================
    void swap(basic_ospanstream& rhs) {
      stream_type::swap(rhs);
      m_buf.swap(rhs.m_buf);
    }

    buf_type* rdbuf() const noexcept {
      return const_cast<buf_type*>(addressof(m_buf));
    }

    span_type span() { return m_buf.span(); }

    void span(span_type s) { m_buf.span(s); }
  };

  template <typename _CharT, typename _Traits>
  void swap(
    basic_ospanstream<_CharT, _Traits> _x,
    basic_ospanstream<_CharT, _Traits> _y) {
    _x.swap(_y);
  }

  template <typename _CharT, typename _Traits = char_traits<_CharT>>
  class basic_spanstream : public basic_iostream<_CharT, _Traits> {
  public:
    using char_type   = _CharT;
    using int_type    = typename _Traits::int_type;
    using pos_type    = typename _Traits::pos_type;
    using off_type    = typename _Traits::off_type;
    using traits_type = _Traits;

  private:
    using span_type   = span<_CharT>;
    using buf_type    = basic_spanbuf<_CharT, _Traits>;
    using stream_type = basic_iostream<_CharT, _Traits>;

    buf_type m_buf;

  public:
    // Constructors
    // ============
    explicit basic_spanstream(
      std::span<_CharT> _s,
      ios_base::openmode _flags = (ios_base::in | ios_base::out)) :
        stream_type(&m_buf), m_buf(_s, _flags) {}

    basic_spanstream(const basic_spanstream&) = delete;

    basic_spanstream(basic_spanstream&& rhs) :
        stream_type(std::move(rhs.m_buf)), m_buf(std::move(rhs.m_buf)) {
      this->set_rdbuf(&m_buf);
    }

    // Member functions
    // ================
    void swap(basic_spanstream& rhs) {
      stream_type::swap(rhs);
      m_buf.swap(rhs.m_buf);
    }

    buf_type* rdbuf() const noexcept {
      return const_cast<buf_type*>(addressof(m_buf));
    }

    span_type span() { return m_buf.span(); }

    void span(span_type s) { m_buf.span(s); }
  };

  template <typename _CharT, typename _Traits>
  void swap(
    basic_spanstream<_CharT, _Traits> _x,
    basic_spanstream<_CharT, _Traits> _y) {
    _x.swap(_y);
  }

  using spanbuf  = basic_spanbuf<char>;
  using wspanbuf = basic_spanbuf<wchar_t>;

  using ispanstream  = basic_ispanstream<char>;
  using wispanstream = basic_ispanstream<wchar_t>;

  using ospanstream  = basic_ospanstream<char>;
  using wospanstream = basic_ospanstream<wchar_t>;

  using spanstream  = basic_spanstream<char>;
  using wspanstream = basic_spanstream<wchar_t>;
#endif
}  // namespace std
#endif