#include "formatters.h"
#include "../colors.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Estados do parser JSON
typedef enum {
    STATE_NORMAL,
    STATE_STRING,
    STATE_STRING_ESCAPE,
    STATE_NUMBER,
    STATE_KEYWORD
} JsonState;

void format_json(const char *data) {
    if (!data) return;

    int indent = 0;
    int in_string = 0;
    int escape_next = 0;
    const char *p = data;

    // Pula whitespace inicial
    while (*p && isspace(*p)) p++;

    while (*p) {
        if (escape_next) {
            putchar(*p);
            escape_next = 0;
            p++;
            continue;
        }

        if (*p == '\\' && in_string) {
            printf("%s\\", color(YELLOW));
            escape_next = 1;
            p++;
            continue;
        }

        if (*p == '"') {
            if (!in_string) {
                printf("%s\"", color(GREEN));
                in_string = 1;
            } else {
                printf("\"%s", color(RESET));
                in_string = 0;
            }
            p++;
            continue;
        }

        if (in_string) {
            putchar(*p);
            p++;
            continue;
        }

        // Fora de string
        switch (*p) {
            case '{':
            case '[':
                printf("%s%c%s\n", color(BOLD_WHITE), *p, color(RESET));
                indent++;
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                break;

            case '}':
            case ']':
                putchar('\n');
                indent--;
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                printf("%s%c%s", color(BOLD_WHITE), *p, color(RESET));
                break;

            case ':':
                printf("%s:%s ", color(BOLD_WHITE), color(RESET));
                break;

            case ',':
                printf("%s,%s\n", color(BOLD_WHITE), color(RESET));
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                break;

            case ' ':
            case '\t':
            case '\n':
            case '\r':
                // Ignora whitespace extra
                break;

            default:
                // Numeros, true, false, null
                if (isdigit(*p) || *p == '-' || *p == '.') {
                    printf("%s", color(YELLOW));
                    while (*p && (isdigit(*p) || *p == '.' || *p == '-' || *p == '+' || *p == 'e' || *p == 'E')) {
                        putchar(*p);
                        p++;
                    }
                    printf("%s", color(RESET));
                    continue;
                } else if (strncmp(p, "true", 4) == 0) {
                    printf("%strue%s", color(MAGENTA), color(RESET));
                    p += 4;
                    continue;
                } else if (strncmp(p, "false", 5) == 0) {
                    printf("%sfalse%s", color(MAGENTA), color(RESET));
                    p += 5;
                    continue;
                } else if (strncmp(p, "null", 4) == 0) {
                    printf("%snull%s", color(DIM), color(RESET));
                    p += 4;
                    continue;
                } else {
                    putchar(*p);
                }
                break;
        }
        p++;
    }

    printf("%s\n", color(RESET));
}
