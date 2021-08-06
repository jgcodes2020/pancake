#if !defined(_PANCAKETEST_DEBUG_HPP_)
#define _PANCAKETEST_DEBUG_HPP_

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <string>
#include <array>
#include <chrono>
#include <thread>

#include <signal.h>

using namespace std;

// UTILITIES

#include <mathutil.hpp>


// DEBUG PRINTER

inline void stack_trace() {
}

inline void on_signal(int sig) {
  switch (sig) {
    case SIGSEGV: {
      cerr << "\033[0;91;1mRaised \033[0;93;1msegmentation fault\033[0m\n";
    } break;
    case SIGINT: {
      cerr << "\033[0;91;1mProgram terminated by user\033[0m\n";
    } break;
  }
  stack_trace();
  std::exit(1);
}
inline void on_exception() {
  std::exception_ptr exc_ptr = std::current_exception();
  try {
    std::rethrow_exception(exc_ptr);
  }
  catch (const std::exception& e) {
    cerr << "\033[0;91mException of type \033[0;96m" << typeid(e).name() << "\033[0;91m thrown\n";
    cerr << "what(): \033[0m" << e.what() << "\n";
  }
  catch (const std::string& str) {
    cerr << "\033[0;91mException string thrown\n";
    cerr << "value: \033[0m" << str << "\n";
  }
  catch (const char* str) {
    cerr << "\033[0;91mException C-string thrown\n";
    cerr << "value: \033[0m" << str << "\n";
  }
  catch(...) {
    cerr << "\033[0;91mUnknown exception object thrown\033[0m\n";
  }
  stack_trace();
  std::exit(1);
}

inline void debug_handlers() {
  #ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  #endif
  signal(SIGSEGV, on_signal);
  std::set_terminate(on_exception);
}
#endif