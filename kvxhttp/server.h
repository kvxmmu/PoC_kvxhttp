//
// Created by kvxmmu on 2/21/21.
//

#ifndef KVXHTTP_SERVER_H
#define KVXHTTP_SERVER_H

#include "event_loop.h"
#include "threadpool.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "kstring.h"

typedef struct {
    uint16_t port;
    uint32_t listen_host;

    KString *path;
    bool running;
} HTTPServer;

static KString kjoin_path(KString *src, const char *to) {
    KString copy = kstring_copy(src);

    if (kstring_endswith(&copy, '/')) {
        kstring_append(to, src);
    } else {
        kstring_append("/", &copy);
        kstring_append(to, &copy);
    }

    return copy;
}

static HTTPServer create_server(uint16_t port, uint32_t listen_host,
        KString *files_path) {
    HTTPServer server;

    server.port = port;
    server.listen_host = listen_host;
    server.path = files_path;
    server.running = false;

    return server;
}

void start_server(HTTPServer *config);

#endif //KVXHTTP_SERVER_H
