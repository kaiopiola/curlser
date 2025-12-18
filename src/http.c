#include "http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Callback to receive the response body
static size_t write_body_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HttpResponse *resp = (HttpResponse *)userp;

    char *ptr = realloc(resp->body, resp->body_size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Error: out of memory\n");
        return 0;
    }

    resp->body = ptr;
    memcpy(&(resp->body[resp->body_size]), contents, realsize);
    resp->body_size += realsize;
    resp->body[resp->body_size] = 0;

    return realsize;
}

// Callback to receive the response headers
static size_t write_header_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HttpResponse *resp = (HttpResponse *)userp;

    char *ptr = realloc(resp->headers, resp->headers_size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Error: out of memory\n");
        return 0;
    }

    resp->headers = ptr;
    memcpy(&(resp->headers[resp->headers_size]), contents, realsize);
    resp->headers_size += realsize;
    resp->headers[resp->headers_size] = 0;

    return realsize;
}

int http_init(void) {
    return curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK ? 0 : -1;
}

void http_cleanup(void) {
    curl_global_cleanup();
}

// Extrai o Content-Type dos headers
static char* extract_content_type(const char *headers) {
    if (!headers) return NULL;

    const char *ct = strcasestr(headers, "content-type:");
    if (!ct) {
        // Try variation
        ct = strcasestr(headers, "Content-Type:");
    }
    if (!ct) return NULL;

    ct += 13; // Pula "content-type:"

    // Skip whitespace
    while (*ct == ' ' || *ct == '\t') ct++;

    // Find end of line
    const char *end = ct;
    while (*end && *end != '\r' && *end != '\n' && *end != ';') end++;

    size_t len = end - ct;
    char *content_type = malloc(len + 1);
    if (content_type) {
        strncpy(content_type, ct, len);
        content_type[len] = '\0';
    }

    return content_type;
}

HttpResponse* http_request(const HttpRequest *req) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: failed to initialize curl\n");
        return NULL;
    }

    HttpResponse *resp = calloc(1, sizeof(HttpResponse));
    if (!resp) {
        curl_easy_cleanup(curl);
        return NULL;
    }

    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, req->url);

    // Set HTTP method
    if (req->method) {
        if (strcmp(req->method, "POST") == 0) {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        } else if (strcmp(req->method, "PUT") == 0) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        } else if (strcmp(req->method, "DELETE") == 0) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        } else if (strcmp(req->method, "PATCH") == 0) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        } else if (strcmp(req->method, "HEAD") == 0) {
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        }
    }

    // Set body
    if (req->body) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req->body);
    }

    // Set custom headers
    struct curl_slist *header_list = NULL;
    for (int i = 0; i < req->header_count; i++) {
        header_list = curl_slist_append(header_list, req->headers[i]);
    }
    if (header_list) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    // Set callbacks
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, resp);

    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // User-Agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curlser/1.0");

    // Verbose mode
    if (req->verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    // Execute request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Request error: %s\n", curl_easy_strerror(res));
        http_response_free(resp);
        if (header_list) curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        return NULL;
    }

    // Get status code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp->status_code);

    // Extract content-type
    resp->content_type = extract_content_type(resp->headers);

    // Cleanup
    if (header_list) curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    return resp;
}

void http_response_free(HttpResponse *resp) {
    if (resp) {
        free(resp->body);
        free(resp->headers);
        free(resp->content_type);
        free(resp);
    }
}
