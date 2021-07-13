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

#include <windows.h>
#include <signal.h>
#include <DbgHelp.h>

using namespace std;

// UTILITIES

#include "mathutil.hpp"


// DEBUG PRINTER

inline void stack_trace() {
  void*          stack[50];
  uint16_t       frames;
  SYMBOL_INFO*   symbol;
  IMAGEHLP_LINE* line;
  unsigned long  line_dspl;
  HANDLE         proc;
  
  proc = GetCurrentProcess();
  SymInitialize(proc, nullptr, true);
  
  frames               = CaptureStackBackTrace(0, 100, stack, nullptr);
  symbol               = reinterpret_cast<SYMBOL_INFO*>(calloc(sizeof(SYMBOL_INFO) + 256*sizeof(char), 1));
  symbol->MaxNameLen   = 50;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  line                 = reinterpret_cast<IMAGEHLP_LINE*>(calloc(sizeof(IMAGEHLP_LINE), 1));
  
  // Find line maximums first
  unsigned max_name_len = 0;
  unsigned max_src_len = 0;
  for (size_t i = 0; i < frames; i++) {
    SymFromAddr(proc, (uint64_t) (stack[i]), 0, symbol);
    SymGetLineFromAddr(proc, (uint64_t) (stack[i]), &line_dspl, line);
    // Max name length
    if (symbol->NameLen > max_name_len) max_name_len = symbol->NameLen; 
    // Max source len
    string src = line->FileName;
    src = src.substr(src.rfind('\\') + 1);
    unsigned src_len = src.length() + 2 + (fast_ilog10(line->LineNumber) + 1);
    if (src_len > max_src_len) max_src_len = src_len;
  }
  uint16_t max_digits = fast_ilog10(frames) + 1;
  
  clog << "━"_s * (max_digits + 1) << "┯" << "━"_s * ((max_name_len + 4) + (max_src_len + 1) + 1) << "┑\n";
  
  for (size_t i = 0; i < frames; i++) {
    SymFromAddr(proc, (uint64_t) (stack[i]), 0, symbol);
    SymGetLineFromAddr(proc, (uint64_t) (stack[i]), &line_dspl, line);
    auto f = clog.flags();
    // Stack pos
    clog << setfill(' ') << setw(max_digits) << right << (frames - i - 1) << " │ ";
    // Symbol name
    clog << setw(max_name_len) << left << symbol->Name << " in ";
    // Source file and line
    string fname = line->FileName;
    fname = fname.substr(fname.rfind('\\') + 1);
    stringstream fmt;
    fmt << fname << ":" << line->LineNumber;
    clog << setw(max_src_len) << left << fmt.str() << " │\n";
    clog.flags(f);
  }
  clog << "━"_s * (max_digits + 1) << "┷" << "━"_s * ((max_name_len + 4) + (max_src_len + 1) + 1) << "┙\n";
  
  free(symbol);
  free(line);
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
  std::exit(0x5E6DEAD);
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
  stack_trace();
  std::exit(0xE2CDEAD);
}

inline void debug_handlers() {
  SetConsoleOutputCP(CP_UTF8);
  signal(SIGSEGV, on_signal);
  std::set_terminate(on_exception);
}
#endif