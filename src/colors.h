#ifndef COLORS_H
#define COLORS_H

// Reset
#define RESET       "\033[0m"

// Regular Colors
#define BLACK       "\033[0;30m"
#define RED         "\033[0;31m"
#define GREEN       "\033[0;32m"
#define YELLOW      "\033[0;33m"
#define BLUE        "\033[0;34m"
#define MAGENTA     "\033[0;35m"
#define CYAN        "\033[0;36m"
#define WHITE       "\033[0;37m"

// Bold
#define BOLD_BLACK      "\033[1;30m"
#define BOLD_RED        "\033[1;31m"
#define BOLD_GREEN      "\033[1;32m"
#define BOLD_YELLOW     "\033[1;33m"
#define BOLD_BLUE       "\033[1;34m"
#define BOLD_MAGENTA    "\033[1;35m"
#define BOLD_CYAN       "\033[1;36m"
#define BOLD_WHITE      "\033[1;37m"

// Dim
#define DIM         "\033[2m"

// Check if colors should be disabled (for piping)
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define STDOUT_FILENO 1
#else
#include <unistd.h>
#endif

static int colors_enabled = 1;

static inline void init_colors(void) {
    colors_enabled = isatty(STDOUT_FILENO);
}

static inline const char* color(const char* c) {
    return colors_enabled ? c : "";
}

#endif // COLORS_H
