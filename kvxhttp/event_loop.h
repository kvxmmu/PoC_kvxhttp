//
// Created by kvxmmu on 2/21/21.
//

#ifndef KVXHTTP_EVENT_LOOP_H
#define KVXHTTP_EVENT_LOOP_H

#include <unistd.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>

#include "map.h"

typedef struct {
    hashmap *fds;

    int epfd;
} EventLoop;

EventLoop event_loop_create();

void      event_loop_set_fd_flags(EventLoop *loop, int fd, uintptr_t flags);
size_t    event_loop_wait(EventLoop *loop, size_t timeout,
                          struct epoll_event *events, int events_count);
void      event_loop_modify(EventLoop *loop, int fd, uintptr_t flags);
void      event_loop_remove(EventLoop *loop, int fd);
void      event_loop_remove_flags(EventLoop *loop, int fd, uintptr_t flags);
void      event_loop_add(EventLoop *loop, int fd, uintptr_t flags);

void      event_loop_destroy(EventLoop *loop);

#endif //KVXHTTP_EVENT_LOOP_H
