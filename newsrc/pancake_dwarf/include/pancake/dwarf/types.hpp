#ifndef _PANCAKE_DWARF_TYPES_
#define _PANCAKE_DWARF_TYPES_
#include <libdwarf/libdwarf.h>
#include <atomic>
#include <cstring>
#include <filesystem>
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
        return std::optional<global>();
      }
      return global(dbg, ptr[value]);
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
    debug(const std::filesystem::path& path);

    operator Dwarf_Debug();

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
  };

  class die final {
    friend class global;

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

    die child() {
      Dwarf_Die res;
      Dwarf_Error err;
      switch (dwarf_child(ptr.get(), &res, &err)) {
        case DW_DLV_NO_ENTRY: {
          throw std::invalid_argument("DIE does not have any children");
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

    die sibling() {
      Dwarf_Die res;
      Dwarf_Error err;
      switch (dwarf_siblingof_b(dbg.get(), ptr.get(), true, &res, &err)) {
        case DW_DLV_NO_ENTRY: {
          throw std::invalid_argument("DIE does not have a sibling");
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
}  // namespace pancake::dwarf
#endif