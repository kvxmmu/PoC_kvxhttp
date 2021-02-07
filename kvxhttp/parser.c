//
// Created by kvxmmu on 2/7/21.
//

#include "parser.h"

http_bool_t http_header_name_cmp(const char *name, struct HttpHeader *header) {
    size_t length = strlen(name);
    header_name_size_t clength = (header->name-1)[0];

    if (length < clength) {
        return 0;
    }

    return strncmp(name, header->name, clength);
}

header_value_size_type_t http_header_get_value_length(struct HttpHeader *header) {
    return *((header_value_size_type_t *)header->value-sizeof(header_value_size_type_t));
}

struct HttpString http_alloc_string(size_t initial_capacity,
                                    const char *initial_value, size_t initial_value_size) {
    struct HttpString string;

    if (initial_capacity == 0) {
        initial_capacity = initial_value_size;
    }

    string.capacity = initial_capacity;
    string.data = calloc(sizeof(char), initial_capacity+1); // for c string compatibility
    string.length = initial_value_size;

    if (initial_value != NULL) {
        memcpy(string.data, initial_value, initial_value_size);
    }

    return string;
}

void http_free_string(struct HttpString *string) {
    string->length = 0;
    string->capacity = 0;

    free(string->data);

    string->data = NULL;
}

http_bool_t http_header_parse(char *data, size_t data_length,
                              struct HttpRequest *request_ptr, size_t max_path_size) {
    uint8_t newline_state = 0;
    uint8_t state = PARSING_HEADLINE;

    uint8_t headline_state = PARSING_METHOD;
    uint8_t header_state = PARSING_KEY;

    size_t start_string_offset = 0;
    size_t offset_size = 0;

    http_bool_t reached_end = 0;

    size_t header_number = 0;

    for (size_t pos = 0; pos < data_length; pos++) {
        char current_char = data[pos];

        if (current_char == '\r' && state == PARSING_HEADERS && header_state == PARSING_KEY) {

            CRLF_PROTOCOL_MISMATCH_CHECK(data)

            reached_end = 1;

            break;
        }

        if ((current_char == ' ' || current_char == '\r') && state == PARSING_HEADLINE) {
            switch (headline_state) {
                case PARSING_METHOD:
                    if (offset_size >= MAX_HTTP_METHOD_SIZE) {
                        return TOO_LONG_METHOD_SIZE;
                    }

                    memcpy(request_ptr->http_method, data, offset_size);

                    start_string_offset = offset_size+1u;
                    offset_size = 0;

                    headline_state = PARSING_PATH;

                    break;

                case PARSING_PATH:
                    if (offset_size >= max_path_size) {
                        return TOO_LONG_PATH_SIZE;
                    }

                    request_ptr->http_path = http_alloc_string(0, data+start_string_offset,
                                                               offset_size);

                    headline_state = PARSING_PROTOCOL;

                    start_string_offset += offset_size+1u;
                    offset_size = 0;

                    break;

                case PARSING_PROTOCOL:
                    if (offset_size >= MAX_HTTP_PROTO_SIZE) {
                        return TOO_LONG_PROTOCOL_SIZE;
                    }

                    memcpy(request_ptr->http_proto, data+start_string_offset,
                           offset_size);

                    start_string_offset += offset_size+1u;
                    offset_size = 0;

                    headline_state = PARSING_END;
                    state = PARSING_HEADERS;

                    break;

                default:
                    return UNHANDLED_EXCEPTION;
            }

            if (current_char == '\r') {
                CRLF_PROTOCOL_MISMATCH_CHECK(data)

                pos++;
                start_string_offset++;

                continue;
            }

            continue;
        } else if (current_char == ':') {
            if (header_number >= MAX_HTTP_HEADERS_COUNT) {
                return TOO_MUCH_HEADERS;
            }

            PROTOCOL_MISMATCH_CHECK()

            if (data[pos+1u] == ' ') {
                pos++;
            }

            if (offset_size >= MAX_KEY_SIZE) {
                return TOO_LONG_HEADER_KEY_SIZE;
            }

            struct HttpHeader *header = &request_ptr->headers[header_number];

            memcpy(header->name, data+start_string_offset,
                    offset_size);

            header_state = PARSING_VALUE;

            start_string_offset += offset_size+2u;
            offset_size = 0;

            continue;
        } else if (current_char == '\r') {
            CRLF_PROTOCOL_MISMATCH_CHECK(data)

            if (offset_size >= MAX_VALUE_SIZE) {
                return TOO_LONG_HEADER_VALUE_SIZE;
            }

            struct HttpHeader *header = &request_ptr->headers[header_number++];

            memcpy(header->value, data+start_string_offset,
                    offset_size);

            pos++;
            start_string_offset += offset_size+2u;

            offset_size = 0;
            header_state = PARSING_KEY;

            continue;
        }

        offset_size++;
    }

    if (!reached_end) {
        return PROTOCOL_MISMATCH;
    }

    request_ptr->headers_count = header_number;

    return SUCCESS;
}

void http_header_free(struct HttpRequest *request_ptr) {
    http_free_string(&request_ptr->http_path);
}

