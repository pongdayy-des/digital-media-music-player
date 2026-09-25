#ifndef CONSOLE_COLORS_H
#define CONSOLE_COLORS_H

#ifdef _WIN32
#include <windows.h>
#endif

// Console (colors)
// ---------------------------------------------------------------------------
// Small, dependency-free helper for ANSI/VT100 color codes in the terminal.
// Implemented with plain std::cout output of standard escape sequences -
// no external library, so the project stays 100% Standard-Library-only as
// required by the assignment (this is still a console app, not a GUI).
//
// On Linux/macOS terminals, ANSI escape codes work out of the box.
// On Windows, classic cmd.exe (conhost) needs "virtual terminal processing"
// explicitly turned on before it will render escape codes as colors instead
// of printing raw control characters; enableVirtualTerminalOutput() does
// this once at program startup. It is a no-op (and compiles to nothing) on
// non-Windows platforms, guarded by #ifdef _WIN32.
namespace Console {

inline void enableVirtualTerminalOutput() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

// Plain ANSI escape sequences. Declared as inline constexpr (C++17 inline
// variables) so this header can be included from multiple translation units
// without violating the One Definition Rule.
inline constexpr const char* RESET   = "\033[0m";

inline constexpr const char* RED     = "\033[31m";
inline constexpr const char* GREEN   = "\033[32m";
inline constexpr const char* YELLOW  = "\033[33m";
inline constexpr const char* CYAN    = "\033[36m";

inline constexpr const char* BOLD_CYAN    = "\033[1;36m";
inline constexpr const char* BOLD_GREEN   = "\033[1;32m";
inline constexpr const char* BOLD_YELLOW  = "\033[1;33m";
inline constexpr const char* BOLD_RED     = "\033[1;31m";
inline constexpr const char* BOLD_MAGENTA = "\033[1;35m";
inline constexpr const char* BOLD_WHITE   = "\033[1;37m";
inline constexpr const char* DIM_WHITE    = "\033[2;37m";

} // namespace Console

#endif // CONSOLE_COLORS_H