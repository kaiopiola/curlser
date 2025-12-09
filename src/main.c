#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "http.h"
#include "colors.h"
#include "formatters/formatters.h"

#define VERSION "1.0.0"
#define MAX_HEADERS 64

static void print_usage(const char *prog) {
    printf("Usage: %s [options] <URL>\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("  -X, --request <METHOD>  HTTP method (GET, POST, PUT, DELETE, etc)\n");
    printf("  -H, --header <HEADER>   Custom header (can be used multiple times)\n");
    printf("  -d, --data <DATA>       Data to send in request body\n");
    printf("  -i, --include           Include response headers in output\n");
    printf("  -r, --raw               Raw output, no formatting\n");
    printf("  -v, --verbose           Verbose mode\n");
    printf("  -h, --help              Show this help\n");
    printf("  -V, --version           Show version\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s https://api.example.com/data\n", prog);
    printf("  %s -X POST -H \"Content-Type: application/json\" -d '{\"key\":\"value\"}' https://api.example.com/create\n", prog);
    printf("  %s -i https://api.example.com/data\n", prog);
}

static void print_version(void) {
    printf("curlser version %s\n", VERSION);
    printf("A CLI tool for HTTP requests with automatic formatting\n");
}

static void print_status(long status_code) {
    const char *status_color;

    if (status_code >= 200 && status_code < 300) {
        status_color = GREEN;
    } else if (status_code >= 300 && status_code < 400) {
        status_color = YELLOW;
    } else if (status_code >= 400 && status_code < 500) {
        status_color = RED;
    } else if (status_code >= 500) {
        status_color = BOLD_RED;
    } else {
        status_color = WHITE;
    }

    printf("%sHTTP Status: %ld%s\n\n", color(status_color), status_code, color(RESET));
}

static void print_headers(const char *headers) {
    if (!headers) return;

    const char *p = headers;
    while (*p) {
        // Linha de status HTTP
        if (strncmp(p, "HTTP/", 5) == 0) {
            printf("%s", color(BOLD_CYAN));
            while (*p && *p != '\r' && *p != '\n') {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
        }
        // Nome do header
        else if (isalpha(*p) || *p == '-') {
            printf("%s", color(CYAN));
            while (*p && *p != ':') {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));

            if (*p == ':') {
                printf("%s:%s", color(BOLD_WHITE), color(RESET));
                p++;
            }

            // Valor do header
            printf("%s", color(WHITE));
            while (*p && *p != '\r' && *p != '\n') {
                putchar(*p);
                p++;
            }
            printf("%s", color(RESET));
        }

        // Avanca para proxima linha
        while (*p == '\r' || *p == '\n') {
            putchar(*p);
            p++;
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // Inicializa cores
    init_colors();

    // Opcoes
    const char *method = "GET";
    const char *headers[MAX_HEADERS];
    int header_count = 0;
    const char *data = NULL;
    int show_headers = 0;
    int raw_output = 0;
    int verbose = 0;

    // Definicao das opcoes longas
    static struct option long_options[] = {
        {"request", required_argument, 0, 'X'},
        {"header",  required_argument, 0, 'H'},
        {"data",    required_argument, 0, 'd'},
        {"include", no_argument,       0, 'i'},
        {"raw",     no_argument,       0, 'r'},
        {"verbose", no_argument,       0, 'v'},
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'V'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "X:H:d:irvhV", long_options, NULL)) != -1) {
        switch (opt) {
            case 'X':
                method = optarg;
                break;
            case 'H':
                if (header_count < MAX_HEADERS) {
                    headers[header_count++] = optarg;
                } else {
                    fprintf(stderr, "Error: maximum number of headers exceeded (%d)\n", MAX_HEADERS);
                }
                break;
            case 'd':
                data = optarg;
                break;
            case 'i':
                show_headers = 1;
                break;
            case 'r':
                raw_output = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'V':
                print_version();
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Check if URL was provided
    if (optind >= argc) {
        fprintf(stderr, "%sError: URL not specified%s\n\n", color(RED), color(RESET));
        print_usage(argv[0]);
        return 1;
    }

    const char *url = argv[optind];

    // Initialize HTTP
    if (http_init() != 0) {
        fprintf(stderr, "%sError: failed to initialize HTTP library%s\n", color(RED), color(RESET));
        return 1;
    }

    // Configura requisicao
    HttpRequest req = {
        .url = url,
        .method = method,
        .headers = headers,
        .header_count = header_count,
        .body = data,
        .show_headers = show_headers,
        .verbose = verbose
    };

    // Executa requisicao
    HttpResponse *resp = http_request(&req);

    if (!resp) {
        http_cleanup();
        return 1;
    }

    // Exibe status
    print_status(resp->status_code);

    // Exibe headers se solicitado
    if (show_headers) {
        print_headers(resp->headers);
    }

    // Exibe body formatado ou raw
    if (resp->body) {
        if (raw_output) {
            printf("%s", resp->body);
        } else {
            format_output(resp->content_type, resp->body);
        }
    }

    // Limpa
    http_response_free(resp);
    http_cleanup();

    return 0;
}
