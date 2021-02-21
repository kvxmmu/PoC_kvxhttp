//
// Created by kvxmmu on 2/21/21.
//

#include "event_loop.h"

EventLoop event_loop_create() {
    EventLoop loop;

    loop.fds = hashmap_create();
    loop.epfd = epoll_create1(0);

    return loop;
}

void event_loop_set_fd_flags(EventLoop *loop, int fd,
        uintptr_t flags) {
    uintptr_t ptr;

    if (hashmap_get(loop->fds, &fd, sizeof(int),
                    &ptr)) {
        ptr |= flags;

        event_loop_modify(loop, fd, ptr);
    } else {
        hashmap_set(loop->fds, &fd, sizeof(int), ptr);
    }
}

void event_loop_modify(EventLoop *loop, int fd, uintptr_t flags) {
    struct epoll_event ev;

    ev.events = flags;
    ev.data.fd = fd;

    if (epoll_ctl(loop->epfd, EPOLL_CTL_MOD, fd, &ev) != 0) {
        perror("epoll_ctl() MOD");

        abort();
    }

    hashmap_set(loop->fds, &fd, sizeof(int), flags);
}

size_t event_loop_wait(EventLoop *loop, size_t timeout,
                          struct epoll_event *events, int events_count) {
    return epoll_wait(loop->epfd, events, events_count, timeout);
}

void event_loop_remove(EventLoop *loop, int fd) {
    uintptr_t events;

    if (!hashmap_get(loop->fds, &fd, sizeof(int), &events)) {
        perror("hashmap_get() remove");

        abort();
    }

    struct epoll_event ev;

    ev.data.fd = fd;
    ev.events = events;

    if (epoll_ctl(loop->epfd, EPOLL_CTL_DEL, fd, &ev) != 0) {
        perror("epoll_ctl() remove");

        abort();
    }

    hashmap_remove(loop->fds, &fd, sizeof(int));
}

void event_loop_remove_flags(EventLoop *loop, int fd, uintptr_t flags) {
    uintptr_t events;

    if (!hashmap_get(loop->fds, &fd, sizeof(int), &events)) {
        perror("hashmap_get() remove flags");

        abort();
    }

    events &= ~flags;

    event_loop_modify(loop, fd, events);
}

void event_loop_add(EventLoop *loop, int fd, uintptr_t flags) {
    uintptr_t events;

    if (!hashmap_get(loop->fds, &fd, sizeof(int), &events)) {
        struct epoll_event ev;

        ev.data.fd = fd;
        ev.events = flags;

        if (epoll_ctl(loop->epfd, EPOLL_CTL_ADD, fd, &ev) != 0) {
            perror("epoll_ctl() add");

            abort();
        }

        hashmap_set(loop->fds, &fd, sizeof(int), flags);
    } else {
        event_loop_modify(loop, fd, events | flags);
    }
}

void event_loop_destroy(EventLoop *loop) {
    hashmap_free(loop->fds);
}