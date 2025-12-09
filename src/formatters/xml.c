#include "formatters.h"
#include "../colors.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void format_xml(const char *data) {
    if (!data) return;

    int indent = 0;
    int in_tag = 0;
    int in_string = 0;
    int in_comment = 0;
    int in_cdata = 0;
    int is_closing_tag = 0;
    int is_self_closing = 0;
    int tag_has_content = 0;
    const char *p = data;

    while (*p) {
        // Detecta comentarios XML
        if (!in_string && strncmp(p, "<!--", 4) == 0) {
            printf("%s<!--", color(DIM));
            p += 4;
            in_comment = 1;
            continue;
        }

        if (in_comment) {
            if (strncmp(p, "-->", 3) == 0) {
                printf("-->%s", color(RESET));
                p += 3;
                in_comment = 0;
                continue;
            }
            putchar(*p);
            p++;
            continue;
        }

        // Detecta CDATA
        if (!in_string && strncmp(p, "<![CDATA[", 9) == 0) {
            printf("%s<![CDATA[", color(YELLOW));
            p += 9;
            in_cdata = 1;
            continue;
        }

        if (in_cdata) {
            if (strncmp(p, "]]>", 3) == 0) {
                printf("]]>%s", color(RESET));
                p += 3;
                in_cdata = 0;
                continue;
            }
            putchar(*p);
            p++;
            continue;
        }

        // Dentro de string (atributo)
        if (in_tag && (*p == '"' || *p == '\'')) {
            char quote = *p;
            if (!in_string) {
                printf("%s%c", color(GREEN), quote);
                in_string = 1;
            } else if (in_string) {
                printf("%c%s", quote, color(RESET));
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

        // Inicio de tag
        if (*p == '<') {
            in_tag = 1;
            is_closing_tag = (*(p + 1) == '/');
            is_self_closing = 0;

            if (is_closing_tag) {
                indent--;
            }

            // Nova linha e indentacao para tags (exceto primeira)
            if (p != data && !tag_has_content) {
                putchar('\n');
                for (int i = 0; i < indent * 2; i++) putchar(' ');
            }

            printf("%s<%s", color(BLUE), color(RESET));

            if (is_closing_tag) {
                printf("%s/", color(BLUE));
                p += 2;
                // Imprime nome da tag
                printf("%s", color(CYAN));
                while (*p && *p != '>' && !isspace(*p)) {
                    putchar(*p);
                    p++;
                }
                printf("%s", color(RESET));
                continue;
            }

            p++;
            // Detecta declaracao XML ou processing instruction
            if (*p == '?') {
                printf("%s?", color(MAGENTA));
                p++;
            }
            // Imprime nome da tag
            printf("%s", color(CYAN));
            while (*p && *p != '>' && *p != '/' && !isspace(*p)) {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
            tag_has_content = 0;
            continue;
        }

        // Fim de tag
        if (*p == '>') {
            // Verifica se e self-closing
            if (p > data && *(p - 1) == '/') {
                is_self_closing = 1;
            } else if (p > data && *(p - 1) == '?') {
                is_self_closing = 1;
            }

            printf("%s>%s", color(BLUE), color(RESET));
            in_tag = 0;

            if (!is_closing_tag && !is_self_closing) {
                indent++;
            }

            tag_has_content = 0;
            p++;
            continue;
        }

        // Self-closing slash
        if (in_tag && *p == '/') {
            printf("%s/", color(BLUE));
            is_self_closing = 1;
            p++;
            continue;
        }

        // Nome de atributo
        if (in_tag && isalpha(*p)) {
            printf(" %s", color(YELLOW));
            while (*p && *p != '=' && *p != '>' && !isspace(*p)) {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
            continue;
        }

        // Igual em atributo
        if (in_tag && *p == '=') {
            printf("%s=%s", color(BOLD_WHITE), color(RESET));
            p++;
            continue;
        }

        // Conteudo de texto
        if (!in_tag) {
            // Pula whitespace entre tags
            if (isspace(*p)) {
                // Verifica se ha conteudo real apos whitespace
                const char *look = p;
                while (*look && isspace(*look)) look++;
                if (*look == '<') {
                    p = look;
                    continue;
                }
            }

            // Conteudo de texto real
            printf("%s", color(WHITE));
            tag_has_content = 1;
            while (*p && *p != '<') {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
            continue;
        }

        // Espacos dentro de tag
        if (in_tag && isspace(*p)) {
            p++;
            continue;
        }

        putchar(*p);
        p++;
    }

    printf("%s\n", color(RESET));
}
