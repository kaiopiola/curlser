#ifndef FORMATTERS_H
#define FORMATTERS_H

// Tipos de conteudo suportados
typedef enum {
    CONTENT_JSON,
    CONTENT_XML,
    CONTENT_HTML,
    CONTENT_TEXT,
    CONTENT_UNKNOWN
} ContentType;

// Detecta o tipo de conteudo baseado no Content-Type header
ContentType detect_content_type(const char *content_type);

// Formata e imprime JSON com syntax highlighting
void format_json(const char *data);

// Formata e imprime XML com syntax highlighting
void format_xml(const char *data);

// Formata e imprime HTML com syntax highlighting
void format_html(const char *data);

// Formata e imprime texto simples
void format_text(const char *data);

// Funcao principal que detecta e formata automaticamente
void format_output(const char *content_type, const char *data);

#endif // FORMATTERS_H
