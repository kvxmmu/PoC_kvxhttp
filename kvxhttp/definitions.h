//
// Created by kvxmmu on 2/7/21.
//

#ifndef KVXHTTP_DEFINITIONS_H
#define KVXHTTP_DEFINITIONS_H

#define MAX_HTTP_HEADERS_COUNT 25u
#define MAX_HTTP_PROTO_SIZE 30u
#define MAX_HTTP_METHOD_SIZE 25u

#define MAX_KEY_SIZE 40u
#define MAX_VALUE_SIZE (MAX_KEY_SIZE<<3u)

#define TOO_LONG_METHOD_SIZE       0b00000001u
#define TOO_LONG_PROTOCOL_SIZE     0b00000010u
#define TOO_LONG_PATH_SIZE         0b00000100u
#define TOO_LONG_HEADER_KEY_SIZE   0b00001000u
#define TOO_LONG_HEADER_VALUE_SIZE 0b00010000u
#define PROTOCOL_MISMATCH          0b00100000u
#define TOO_MUCH_HEADERS           0b01000000u
#define SUCCESS                    0b00000000u


#define PROTOCOL_MISMATCH_CHECK() if (pos+1u >= data_length) {\
                                    return PROTOCOL_MISMATCH;\
                                  }

#define CRLF_PROTOCOL_MISMATCH_CHECK(data) PROTOCOL_MISMATCH_CHECK() \
                                           if (data[pos+1u] != '\n') { \
                                            return PROTOCOL_MISMATCH; \
                                           }

#define UNHANDLED_EXCEPTION 0xff

/* States */

#define PARSING_HEADLINE 1u
#define PARSING_HEADERS 2u

/* Headline states */
#define PARSING_METHOD 0u
#define PARSING_PATH 1u
#define PARSING_PROTOCOL 2u
#define PARSING_END 3u

/* Header state */
#define PARSING_KEY 0u
#define PARSING_VALUE 1u

#endif //KVXHTTP_DEFINITIONS_H
