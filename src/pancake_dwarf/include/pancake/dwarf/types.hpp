#ifndef _PANCAKE_DWARF_TYPES_
#define _PANCAKE_DWARF_TYPES_
#include <libdwarf/libdwarf.h>
#include <atomic>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <typeinfo>

#include <pancake/dwarf/enums.hpp>

namespace pancake::dwarf {
  namespace details {
    template <typename...>
    constexpr bool false_sink = false;
  }
  class debug;
  class global;
  class die;

  template <typename T>
  class array;

  class global final {
    friend class array<global>;

  private:
    std::shared_ptr<Dwarf_Debug_s> dbg;
    Dwarf_Global ptr;

    global(const std::shared_ptr<Dwarf_Debug_s>& dbg_p, Dwarf_Global ptr_p) :
        dbg(dbg_p), ptr(ptr_p) {}

  public:
    std::string name() {
      Dwarf_Error err;
      char* result;
      if (dwarf_globname(ptr, &result, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }

      return result;
    }

    die die();
  };

  class global_type final {
    friend class array<global_type>;

  private:
    std::shared_ptr<Dwarf_Debug_s> dbg;
    Dwarf_Type ptr;

    global_type(const std::shared_ptr<Dwarf_Debug_s>& dbg_p, Dwarf_Type ptr_p) :
        dbg(dbg_p), ptr(ptr_p) {}

  public:
    std::string name() {
      Dwarf_Error err;
      char* result;
      if (dwarf_pubtypename(ptr, &result, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }

      return result;
    }

    die die();
  };

  template <>
  class array<global> final {
    friend class debug;

  private:
    struct deleter {
      std::shared_ptr<Dwarf_Debug_s> dbg;
      Dwarf_Signed size;
      void operator()(Dwarf_Global* array) {
        dwarf_globals_dealloc(dbg.get(), array, size);
      }
    };
    std::shared_ptr<Dwarf_Debug_s> dbg;
    std::shared_ptr<Dwarf_Global[]> ptr;
    Dwarf_Signed size;

    array(
      const std::shared_ptr<Dwarf_Debug_s>& dbg_p, Dwarf_Global* ptr_p,
      Dwarf_Signed size_p) :
        dbg(dbg_p), ptr(ptr_p, deleter {dbg, size_p}), size(size_p) {}

  public:
    global operator[](size_t i) {
      if (i > size) {
        throw std::out_of_range("Accessed out-of-range in global array");
      }
      return global(dbg, ptr[i]);
    }

    std::optional<global> find(std::string_view sym) {
      using ssize_t = std::make_signed_t<size_t>;

      Dwarf_Error err;
      ssize_t value = -1;
      for (Dwarf_Signed i = 0; i < size; i++) {
        char* str;
        if (dwarf_globname(ptr[i], &str, &err) == DW_DLV_ERROR) {
          throw std::logic_error(dwarf_errmsg(err));
        }

        if (sym == str) {
          value = i;
          break;
        }
      }
      if (value < 0) {
        return std::nullopt;
      }
      return global(dbg, ptr[value]);
    }
  };
  
  template <>
  class array<global_type> final {
    friend class debug;

  private:
    struct deleter {
      std::shared_ptr<Dwarf_Debug_s> dbg;
      Dwarf_Signed size;
      void operator()(Dwarf_Type* array) {
        dwarf_pubtypes_dealloc(dbg.get(), array, size);
      }
    };
    std::shared_ptr<Dwarf_Debug_s> dbg;
    std::shared_ptr<Dwarf_Type[]> ptr;
    Dwarf_Signed size;

    array(
      const std::shared_ptr<Dwarf_Debug_s>& dbg_p, Dwarf_Type* ptr_p,
      Dwarf_Signed size_p) :
        dbg(dbg_p), ptr(ptr_p, deleter {dbg, size_p}), size(size_p) {}

  public:
    global_type operator[](size_t i) {
      if (i > size) {
        throw std::out_of_range("Accessed out-of-range in global array");
      }
      return global_type(dbg, ptr[i]);
    }

    std::optional<global_type> find(std::string_view sym) {
      using ssize_t = std::make_signed_t<size_t>;

      Dwarf_Error err;
      ssize_t value = -1;
      for (Dwarf_Signed i = 0; i < size; i++) {
        char* str;
        if (dwarf_pubtypename(ptr[i], &str, &err) == DW_DLV_ERROR) {
          throw std::logic_error(dwarf_errmsg(err));
        }

        if (sym == str) {
          value = i;
          break;
        }
      }
      if (value < 0) {
        return std::nullopt;
      }
      return global_type(dbg, ptr[value]);
    }
  };

  class debug final {
    friend class die;
    friend class array<global>;

  private:
    struct deleter {
      void operator()(Dwarf_Debug dbg) { dwarf_finish(dbg, nullptr); }
    };
    std::shared_ptr<Dwarf_Debug_s> ptr;

  public:
    debug(const std::filesystem::path& path) :
        ptr(
          [&]() -> Dwarf_Debug {
            Dwarf_Debug res;
            Dwarf_Error err;
            switch (dwarf_init_path(
              path.string().c_str(), nullptr, 0, 0, nullptr, nullptr, &res,
              &err)) {
              case DW_DLV_ERROR: {
                throw std::invalid_argument(dwarf_errmsg(err));
              } break;
              case DW_DLV_NO_ENTRY: {
                throw std::invalid_argument("File does not exist");
              } break;
            }
            return res;
          }(),
          deleter {}) {}

    array<global> globals() {
      Dwarf_Error err;
      Dwarf_Global* arr;
      Dwarf_Signed size;
      switch (dwarf_get_globals(ptr.get(), &arr, &size, &err)) {
        case DW_DLV_NO_ENTRY: {
          throw std::logic_error(
            "No .debug_pubnames section exists in this binary");
        } break;
        case DW_DLV_ERROR: {
          throw std::invalid_argument(dwarf_errmsg(err));
        }
        case DW_DLV_OK: {
          return array<global>(ptr, arr, size);
        }
      }
      throw std::logic_error("This shouldn't happen");
    }
    
    array<global_type> global_types() {
      Dwarf_Error err;
      Dwarf_Type* arr;
      Dwarf_Signed size;
      switch (dwarf_get_pubtypes(ptr.get(), &arr, &size, &err)) {
        case DW_DLV_NO_ENTRY: {
          throw std::logic_error(
            "No .debug_pubtypes section exists in this binary");
        } break;
        case DW_DLV_ERROR: {
          throw std::invalid_argument(dwarf_errmsg(err));
        }
        case DW_DLV_OK: {
          return array<global_type>(ptr, arr, size);
        }
      }
      throw std::logic_error("This shouldn't happen");
    }
  };

  class die final {
    friend class global;
    friend class global_type;

  private:
    struct deleter {
      void operator()(Dwarf_Die die) { dwarf_dealloc_die(die); }
    };
    std::shared_ptr<Dwarf_Debug_s> dbg;
    std::shared_ptr<Dwarf_Die_s> ptr;

    die(const std::shared_ptr<Dwarf_Debug_s>& dbg_p, Dwarf_Die die) :
        dbg(dbg_p), ptr(die, deleter()) {}

    Dwarf_Attribute attr_obj(dw_attrs attr) {
      Dwarf_Attribute att;
      Dwarf_Error err;
      switch (
        dwarf_attr(ptr.get(), static_cast<Dwarf_Half>(attr), &att, &err)) {
        case DW_DLV_NO_ENTRY: {
          throw std::invalid_argument("Attribute does not exist");
        } break;
        case DW_DLV_ERROR: {
          throw std::invalid_argument(dwarf_errmsg(err));
        }
        case DW_DLV_OK: {
          return att;
        }
      }
      throw std::logic_error("this shouldn't happen");
    }

  public:
    bool has_attr(dw_attrs type) {
      Dwarf_Error err;
      Dwarf_Bool res;
      dwarf_hasattr(ptr.get(), static_cast<Dwarf_Half>(type), &res, &err);
      return res;
    }

    template <typename T>
    T get_attr(dw_attrs type);

    template <>
    Dwarf_Unsigned get_attr(dw_attrs type) {
      Dwarf_Attribute attr = attr_obj(type);

      Dwarf_Error err;
      Dwarf_Unsigned res;
      if (dwarf_formudata(attr, &res, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }
      dwarf_dealloc_attribute(attr);

      return res;
    }

    template <>
    Dwarf_Signed get_attr(dw_attrs type) {
      Dwarf_Attribute attr = attr_obj(type);

      Dwarf_Error err;
      Dwarf_Signed res;
      if (dwarf_formsdata(attr, &res, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }
      dwarf_dealloc_attribute(attr);

      return res;
    }

    template <>
    std::string get_attr(dw_attrs type) {
      Dwarf_Attribute attr = attr_obj(type);

      Dwarf_Error err;
      char* res;
      if (dwarf_formstring(attr, &res, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }
      dwarf_dealloc_attribute(attr);

      return res;
    }

    template <>
    die get_attr(dw_attrs type) {
      Dwarf_Attribute attr = attr_obj(type);

      Dwarf_Error err;
      Dwarf_Off off;
      if (dwarf_global_formref(attr, &off, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }
      dwarf_dealloc_attribute(attr);

      Dwarf_Die res;
      if (dwarf_offdie_b(dbg.get(), off, true, &res, &err) == DW_DLV_ERROR) {
        throw std::invalid_argument(dwarf_errmsg(err));
      }

      return die(dbg, res);
    }

    die_tag tag() {
      Dwarf_Error err;
      Dwarf_Half res;
      if (dwarf_tag(ptr.get(), &res, &err) == DW_DLV_ERROR) {
        throw std::logic_error(dwarf_errmsg(err));
      }

      return static_cast<die_tag>(res);
    }

    std::optional<die> child() {
      Dwarf_Die res;
      Dwarf_Error err;
      switch (dwarf_child(ptr.get(), &res, &err)) {
        case DW_DLV_NO_ENTRY: {
          return std::nullopt;
        } break;
        case DW_DLV_ERROR: {
          throw std::invalid_argument(dwarf_errmsg(err));
        } break;
        case DW_DLV_OK: {
          return die(dbg, res);
        } break;
      }
      throw std::logic_error("this shouldn't happen");
    }

    std::optional<die> sibling() {
      Dwarf_Die res;
      Dwarf_Error err;
      switch (dwarf_siblingof_b(dbg.get(), ptr.get(), true, &res, &err)) {
        case DW_DLV_NO_ENTRY: {
          return std::nullopt;
        } break;
        case DW_DLV_ERROR: {
          throw std::invalid_argument(dwarf_errmsg(err));
        } break;
        case DW_DLV_OK: {
          return die(dbg, res);
        } break;
      }
      throw std::logic_error("this shouldn't happen");
    }

    friend std::ostream& operator<<(std::ostream& out, die& die) {
      out << "DIE of type " << die.tag() << "\n";

      Dwarf_Error err;
      Dwarf_Attribute* attrs;
      Dwarf_Signed size;
#define dw_check(fn)      \
  if (fn == DW_DLV_ERROR) \
  throw std::invalid_argument(dwarf_errmsg(err))

      dw_check(dwarf_attrlist(die.ptr.get(), &attrs, &size, &err));

      for (size_t i = 0; i < size; i++) {
        Dwarf_Half name;
        dw_check(dwarf_whatattr(attrs[i], &name, &err));
        out << static_cast<dw_attrs>(name) << ": ";
        Dwarf_Half form;
        dw_check(dwarf_whatform(attrs[i], &form, &err));
        switch (static_cast<attr_form>(form)) {
          case attr_form::udata:
          case attr_form::data1:
          case attr_form::data2:
          case attr_form::data4:
          case attr_form::data8: {
            Dwarf_Unsigned res;
            dw_check(dwarf_formudata(attrs[i], &res, &err));
            out << res << "\n";
          } break;
          case attr_form::sdata: {
            Dwarf_Signed res;
            dw_check(dwarf_formsdata(attrs[i], &res, &err));
            out << res << "\n";
          } break;
          case attr_form::string:
          case attr_form::strp: {
            char* res;
            dw_check(dwarf_formstring(attrs[i], &res, &err));
            out << res << "\n";
          } break;
          case attr_form::ref1:
          case attr_form::ref2:
          case attr_form::ref4:
          case attr_form::ref8: {
            Dwarf_Off res;
            dw_check(dwarf_global_formref(attrs[i], &res, &err));
            auto flags = out.flags();
            out << "<DIE at " << std::hex << res;
            out.flags(flags);
            out << ">\n";
          } break;
          default: {
            out << "<didn't parse: form " << form << ">\n";
          } break;
        }
      }

#undef dw_check
      return out;
    }
  };

  inline die global::die() {
    namespace dwarf = pancake::dwarf;

    Dwarf_Error err;
    Dwarf_Off off;
    if (dwarf_global_die_offset(ptr, &off, &err) == DW_DLV_ERROR) {
      throw std::invalid_argument(dwarf_errmsg(err));
    }

    Dwarf_Die res;
    if (dwarf_offdie_b(dbg.get(), off, true, &res, &err) == DW_DLV_ERROR) {
      throw std::invalid_argument(dwarf_errmsg(err));
    }

    return dwarf::die(dbg, res);
  }

  inline die global_type::die() {
    namespace dwarf = pancake::dwarf;

    Dwarf_Error err;
    Dwarf_Off off;
    if (dwarf_pubtype_type_die_offset(ptr, &off, &err) == DW_DLV_ERROR) {
      throw std::invalid_argument(dwarf_errmsg(err));
    }

    Dwarf_Die res;
    if (dwarf_offdie_b(dbg.get(), off, true, &res, &err) == DW_DLV_ERROR) {
      throw std::invalid_argument(dwarf_errmsg(err));
    }

    return dwarf::die(dbg, res);
  }
}  // namespace pancake::dwarf
#endif