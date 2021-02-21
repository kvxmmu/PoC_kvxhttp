//
// Created by kvxmmu on 2/21/21.
//

#include "server.h"

#define MAX_LISTEN 4096u<<3u

void start_server(HTTPServer *config) {
    int y = 1;
    struct epoll_event *events = calloc(MAX_LISTEN, sizeof(struct epoll_event));

    config->running = true;

    EventLoop loop = event_loop_create();

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = config->listen_host;
    addr.sin_port = htons(config->port);
    addr.sin_family = AF_INET;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    int status = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if (status != 0) {
        perror("bind()");

        abort();
    }

    status = listen(sockfd, MAX_LISTEN);

    if (status != 0) {
        perror("listen()");

        abort();
    }

    event_loop_add(&loop, sockfd, EPOLLIN);

    while (config->running) {
        size_t nfds = event_loop_wait(&loop, 0, events, MAX_LISTEN);

        for (size_t nfd = 0; nfd < nfds; nfd++) {
            struct epoll_event event = events[nfd];

            // TODO: complete this
        }
    }

    event_loop_destroy(&loop);

    close(sockfd);
    free(events);
}