#ifndef COLOR_H
#define COLOR_H

#include <cstdio>
#include <string>

#if defined(_WIN32)
#include <io.h>
#define TRADING_ISATTY _isatty
#define TRADING_FILENO _fileno
#else
#include <unistd.h>
#define TRADING_ISATTY isatty
#define TRADING_FILENO fileno
#endif

namespace Service {

// ANSI terminal colors for console output.
class Color {
public:
    static constexpr const char* Reset = "\033[0m";
    static constexpr const char* Bold = "\033[1m";

    static constexpr const char* Red = "\033[31m";
    static constexpr const char* Green = "\033[32m";
    static constexpr const char* Yellow = "\033[33m";
    static constexpr const char* Blue = "\033[34m";
    static constexpr const char* Magenta = "\033[35m";
    static constexpr const char* Cyan = "\033[36m";
    static constexpr const char* White = "\033[37m";

    static constexpr const char* BrightRed = "\033[91m";
    static constexpr const char* BrightGreen = "\033[92m";
    static constexpr const char* BrightYellow = "\033[93m";
    static constexpr const char* BrightCyan = "\033[96m";

    // True when the given stream is an interactive terminal.
    static bool enabled(FILE* stream = stdout) {
        return TRADING_ISATTY(TRADING_FILENO(stream)) != 0;
    }

    // Wrap text in ANSI codes when color is enabled; plain text otherwise.
    static std::string wrap(const char* code, const std::string& text, FILE* stream = stdout) {
        if (!enabled(stream)) {
            return text;
        }
        return std::string(code) + text + Reset;
    }
};

inline std::string paint(const std::string& text, const char* code, FILE* stream = stdout) {
    return Color::wrap(code, text, stream);
}

} // namespace Service

#undef TRADING_ISATTY
#undef TRADING_FILENO

#endif
