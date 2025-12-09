#include "formatters.h"
#include "../colors.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Converte string para minusculo para comparacao
static void to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

ContentType detect_content_type(const char *content_type) {
    if (!content_type) return CONTENT_UNKNOWN;

    char ct[256];
    strncpy(ct, content_type, sizeof(ct) - 1);
    ct[sizeof(ct) - 1] = '\0';
    to_lower(ct);

    // JSON
    if (strstr(ct, "application/json") ||
        strstr(ct, "text/json") ||
        strstr(ct, "+json")) {
        return CONTENT_JSON;
    }

    // XML
    if (strstr(ct, "application/xml") ||
        strstr(ct, "text/xml") ||
        strstr(ct, "+xml")) {
        return CONTENT_XML;
    }

    // HTML
    if (strstr(ct, "text/html") ||
        strstr(ct, "application/xhtml")) {
        return CONTENT_HTML;
    }

    // Texto simples
    if (strstr(ct, "text/plain") ||
        strstr(ct, "text/")) {
        return CONTENT_TEXT;
    }

    return CONTENT_UNKNOWN;
}

void format_text(const char *data) {
    if (!data) return;
    printf("%s%s%s\n", color(WHITE), data, color(RESET));
}

void format_output(const char *content_type, const char *data) {
    if (!data) return;

    ContentType type = detect_content_type(content_type);

    switch (type) {
        case CONTENT_JSON:
            format_json(data);
            break;
        case CONTENT_XML:
            format_xml(data);
            break;
        case CONTENT_HTML:
            format_html(data);
            break;
        case CONTENT_TEXT:
        case CONTENT_UNKNOWN:
        default:
            format_text(data);
            break;
    }
}
