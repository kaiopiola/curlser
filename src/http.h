#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

// Estrutura para armazenar resposta HTTP
typedef struct {
    char *body;
    size_t body_size;
    char *headers;
    size_t headers_size;
    long status_code;
    char *content_type;
} HttpResponse;

// Estrutura para configuracao da requisicao
typedef struct {
    const char *url;
    const char *method;
    const char **headers;
    int header_count;
    const char *body;
    int show_headers;
    int verbose;
} HttpRequest;

// Inicializa a biblioteca HTTP
int http_init(void);

// Limpa recursos da biblioteca HTTP
void http_cleanup(void);

// Executa uma requisicao HTTP
HttpResponse* http_request(const HttpRequest *req);

// Libera memoria da resposta
void http_response_free(HttpResponse *resp);

#endif // HTTP_H
