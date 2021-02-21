//
// Created by kvxmmu on 2/7/21.
//

#ifndef KVXHTTP_PARSER_H
#define KVXHTTP_PARSER_H

#include "definitions.h"
#include "kstring.h"

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include <ctype.h>

typedef uint8_t header_name_size_t;
typedef uint16_t header_value_size_type_t; // MAX: 1kb header
typedef uint8_t http_bool_t;

typedef uint8_t headers_count_t;

struct HttpHeader {
    char name[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
};

struct HttpRequest {
    struct HttpHeader headers[MAX_HTTP_HEADERS_COUNT];
    headers_count_t headers_count;

    char http_proto[MAX_HTTP_PROTO_SIZE];
    char http_method[MAX_HTTP_METHOD_SIZE];
    KString http_path;
};


// GET / HTTP/1.1

http_bool_t              http_header_name_cmp(const char *name, struct HttpHeader *header);
header_value_size_type_t http_header_get_value_length(struct HttpHeader *header);

struct HttpString http_alloc_string(size_t initial_capacity,
        const char *initial_value, size_t initial_value_size);
void              http_free_string(struct HttpString *string);

http_bool_t http_header_parse(char *data, size_t data_length,
            struct HttpRequest *request_ptr, size_t max_path_size);
void http_header_free(struct HttpRequest *request_ptr);

// String

char from_hex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}


#endif //KVXHTTP_PARSER_H
