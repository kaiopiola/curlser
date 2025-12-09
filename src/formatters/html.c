#include "formatters.h"
#include "../colors.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Tags que nao precisam de fechamento
static const char *void_tags[] = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr", NULL
};

// Tags que preservam whitespace
static const char *preformatted_tags[] = {
    "pre", "code", "textarea", "script", "style", NULL
};

static int is_void_tag(const char *tag, size_t len) {
    for (int i = 0; void_tags[i]; i++) {
        if (strncasecmp(tag, void_tags[i], len) == 0 && strlen(void_tags[i]) == len) {
            return 1;
        }
    }
    return 0;
}

static int is_preformatted_tag(const char *tag, size_t len) {
    for (int i = 0; preformatted_tags[i]; i++) {
        if (strncasecmp(tag, preformatted_tags[i], len) == 0 && strlen(preformatted_tags[i]) == len) {
            return 1;
        }
    }
    return 0;
}

void format_html(const char *data) {
    if (!data) return;

    int indent = 0;
    int in_tag = 0;
    int in_string = 0;
    int in_comment = 0;
    int in_script = 0;
    int in_style = 0;
    int is_closing_tag = 0;
    int is_doctype = 0;
    int needs_newline = 0;
    char current_tag[64] = {0};
    int tag_name_idx = 0;
    const char *p = data;

    while (*p) {
        // Detecta comentarios HTML
        if (!in_string && strncmp(p, "<!--", 4) == 0) {
            if (needs_newline) {
                putchar('\n');
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                needs_newline = 0;
            }
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
                needs_newline = 1;
                continue;
            }
            putchar(*p);
            p++;
            continue;
        }

        // Conteudo de script/style (sem formatacao)
        if (in_script || in_style) {
            const char *end_tag = in_script ? "</script" : "</style";
            if (strncasecmp(p, end_tag, strlen(end_tag)) == 0) {
                in_script = 0;
                in_style = 0;
            } else {
                printf("%s", color(in_script ? YELLOW : MAGENTA));
                putchar(*p);
                printf("%s", color(RESET));
                p++;
                continue;
            }
        }

        // Dentro de string (atributo)
        if (in_tag && (*p == '"' || *p == '\'')) {
            char quote = *p;
            if (!in_string) {
                printf("%s%c", color(GREEN), quote);
                in_string = 1;
            } else {
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
            is_doctype = (strncasecmp(p + 1, "!doctype", 8) == 0);
            tag_name_idx = 0;
            memset(current_tag, 0, sizeof(current_tag));

            if (is_closing_tag) {
                indent--;
                if (indent < 0) indent = 0;
            }

            if (needs_newline) {
                putchar('\n');
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                needs_newline = 0;
            }

            printf("%s<%s", color(BLUE), color(RESET));

            if (is_doctype) {
                printf("%s!DOCTYPE%s", color(MAGENTA), color(RESET));
                p += 9;
                continue;
            }

            if (is_closing_tag) {
                printf("%s/", color(BLUE));
                p += 2;
            } else {
                p++;
            }

            // Captura nome da tag
            printf("%s", color(CYAN));
            while (*p && *p != '>' && *p != '/' && !isspace(*p)) {
                if (tag_name_idx < 63) {
                    current_tag[tag_name_idx++] = tolower(*p);
                }
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
            continue;
        }

        // Fim de tag
        if (*p == '>') {
            int is_self_closing = (p > data && *(p - 1) == '/');
            size_t tag_len = strlen(current_tag);

            printf("%s>%s", color(BLUE), color(RESET));
            in_tag = 0;

            if (!is_closing_tag && !is_self_closing && !is_void_tag(current_tag, tag_len)) {
                indent++;
            }

            // Detecta script/style
            if (!is_closing_tag && tag_len > 0) {
                if (strcmp(current_tag, "script") == 0) in_script = 1;
                if (strcmp(current_tag, "style") == 0) in_style = 1;
            }

            needs_newline = 1;
            is_doctype = 0;
            p++;
            continue;
        }

        // Self-closing slash
        if (in_tag && *p == '/') {
            printf("%s/", color(BLUE));
            p++;
            continue;
        }

        // Nome de atributo
        if (in_tag && (isalpha(*p) || *p == '-' || *p == '_')) {
            printf(" %s", color(YELLOW));
            while (*p && *p != '=' && *p != '>' && *p != '/' && !isspace(*p)) {
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
                const char *look = p;
                while (*look && isspace(*look)) look++;
                if (*look == '<') {
                    p = look;
                    continue;
                }
                // Whitespace significativo
                p++;
                continue;
            }

            // Conteudo de texto real
            if (needs_newline) {
                putchar('\n');
                for (int i = 0; i < indent * 2; i++) putchar(' ');
                needs_newline = 0;
            }

            printf("%s", color(WHITE));
            while (*p && *p != '<' && !isspace(*p)) {
                putchar(*p);
                p++;
            }
            // Inclui espacos no texto
            while (*p && isspace(*p) && *(p + 1) && *(p + 1) != '<') {
                putchar(' ');
                while (*p && isspace(*p)) p++;
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
