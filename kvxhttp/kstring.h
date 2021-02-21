//
// Created by kvxmmu on 2/17/21.
//

#ifndef KVXHTTP_KSTRING_H
#define KVXHTTP_KSTRING_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_STRING_CAPACITY 5u

typedef struct {
    char *src;

    size_t capacity;
    size_t length;
} KString;

static KString kstring_create(const char *source,
                       size_t capacity_reserve, size_t len) {
    if (capacity_reserve == 0) {
        capacity_reserve = DEFAULT_STRING_CAPACITY;
    }

    KString string;
    if (len == 0) {
        len = strlen(source);
    }

    string.length = len;
    string.capacity = len+capacity_reserve+1u;
    string.src = calloc(string.capacity, sizeof(char));

    memcpy(string.src, source, len);

    return string;
}

static KString kstring_copy(KString *source) {
    KString str;

    str.length = source->length;
    str.capacity = source->capacity;
    str.src = calloc(source->capacity, sizeof(char));

    memcpy(&str.src, source->src, source->length);

    return str;
}

static bool kstring_is_enough_cap(KString *string, size_t for_length) {
    return string->capacity-1u >= for_length;
}

static void kstring_expand(KString *string, size_t expand_to) {
    string->src = realloc(string->src, sizeof(char)*expand_to);
    string->capacity = expand_to;
}

static char kstring_getc(KString *string, size_t index) {
    return string->src[index];
}

static bool kstring_endswith(KString *string, char chr) {
    if (string->length < 1) {
        return false;
    }

    return kstring_getc(string, string->length-1u) == chr;
}

static void kstring_double_till(KString *string, size_t until) {
    while (kstring_is_enough_cap(string, until)) {
        kstring_expand(string, string->capacity<<1u);
    }
}

static void kstring_append(const char *data, KString *string) {
    size_t len = strlen(data);
    size_t need_data = len + string->length;

    if (!kstring_is_enough_cap(string, need_data)) {
        kstring_double_till(string, need_data);
    }

    memcpy(string->src+string->length, data, len);

    string->length += len;
}

static bool kstring_is_freed(KString *string) {
    return string->capacity == 0 && string->length == 0;
}

static void kstring_free(KString *string) {
    free(string->src);

    string->src = NULL;
    string->length = 0;
    string->capacity = 0;
}

#endif //KANSCRIPT_KSTRING_H
