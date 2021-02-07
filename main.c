#include <stdio.h>

#include "kvxhttp/parser.h"

#define ITERCOUNT (10000000u >> 1u)

int main() {
    struct HttpRequest request;

    char *data = "GET / HTTP/1.1\r\n"
                 "Connection: close\r\n"
                 "Content-Length: 1\r\n\r\n"
                 "1";
    size_t data_strlen = strlen(data);

    for (size_t pos = 0; pos < ITERCOUNT; pos++) {
        http_header_parse(data, data_strlen,
                          &request, 200);

        http_header_free(&request);
    }
}
