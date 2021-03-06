#include <pancake/dl/pdl.hpp>

#include <filesystem>
#include <string>

#include <windows.h>
#include <LIEF/PE.hpp>

namespace PE = LIEF::PE;
namespace fs = std::filesystem;

using std::string;

namespace {
  // Credit to Jamin Gray on StackOverflow.
  // https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
  string win_errmsg(DWORD error) {
    // Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
      // If this happens, this is worth it.
      return std::string("Task failed successfully?");
    }

    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the
    // message for us (because we don't yet know how long the message string will
    // be).
    size_t size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, 0, (LPSTR) &messageBuffer, 0, NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
  }
}  // namespace

namespace pancake::dl {
  struct library::impl {
    const handle hnd;
    std::unique_ptr<PE::Binary> bin;

    impl(const fs::path& path) :
        hnd([&path]() {
          handle hnd = LoadLibraryW(path.wstring().c_str());
          if (hnd == nullptr) {
            string error = win_errmsg(GetLastError());
            throw dl_error(error);
          }
          return hnd;
        }()),
        bin(PE::Parser::parse(path.string())) {}

    ~impl() { FreeLibrary(hnd); }

    void* get_symbol(const std::string& name) const {
      void* sym = reinterpret_cast<void*>(GetProcAddress(hnd, name.c_str()));
      if (sym == nullptr) {
        string error = win_errmsg(GetLastError());
        throw dl_error(error);
      }

      return sym;
    }

    section get_section(const std::string& name) const {
      auto sect = bin->get_section(name);

      return section {
        reinterpret_cast<void*>(
          reinterpret_cast<uint8_t*>(hnd) + sect.virtual_address()),
        sect.virtual_size()};
    }
  };

  library::library(const fs::path& path) : p_impl(new impl(path)) {}

  library::~library() = default;

  void* library::_impl_get_symbol(const std::string& str) const {
    return p_impl->get_symbol(str);
  }

  handle library::native_handle() const { return p_impl->hnd; }

  section library::get_section(const std::string& name) const {
    return p_impl->get_section(name);
  }
}  // namespace pancake::dl