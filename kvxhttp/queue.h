//
// Created by kvxmmu on 2/21/21.
//

#ifndef KVXHTTP_QUEUE_H
#define KVXHTTP_QUEUE_H

#include <sys/syscall.h>
#include <pthread.h>

#include <linux/futex.h>

#include <string.h>

#define DEFAULT_INITIAL_CAPACITY 50u

typedef void (*callback_t)(void *);

typedef struct {
    void **queue;

    size_t capacity;
    size_t length;

    size_t ptr;

    pthread_mutex_t mutex;
} ThreadQueue;

static ThreadQueue threadqueue_create(size_t initial_capacity) {
    if (initial_capacity == 0u) {
        initial_capacity = DEFAULT_INITIAL_CAPACITY;
    }

    ThreadQueue queue;

    queue.queue = calloc(initial_capacity, sizeof(void *));
    queue.capacity = initial_capacity;
    queue.length = 0;
    queue.ptr = 0;

    pthread_mutex_init(&queue.mutex, NULL);

    return queue;
}

static void *threadqueue_xpop(ThreadQueue *queue, bool pop) {
    void *cb = queue->queue[queue->ptr];

    queue->queue[queue->ptr] = NULL;

    if (pop) {
        queue->length--;
        queue->ptr++;

        if (queue->ptr >= 0xffu) {
            memmove(queue->queue, queue->queue+queue->ptr,
                    queue->length*sizeof(void *));

            queue->ptr = 0;
        }
    }

    return cb;
}

static void *threadqueue_pop(ThreadQueue *queue, bool pop) {
    while (queue->length == 0) {
        syscall(SYS_futex, &queue->length, FUTEX_WAIT, 0, NULL);
    }

    pthread_mutex_lock(&queue->mutex);

    callback_t callback = threadqueue_xpop(queue, pop);

    pthread_mutex_unlock(&queue->mutex);

    return callback;
}

static void threadqueue_twice_size(ThreadQueue *queue) {
    queue->queue = realloc(queue->queue, sizeof(void *) * (queue->capacity<<1u));

    queue->capacity <<= 1u;
}

static void threadqueue_xpush(ThreadQueue *queue, void * callback) {
    if (queue->capacity < queue->length+1u) {
        threadqueue_twice_size(queue);
    }

    queue->queue[queue->length++] = callback;
}

static void threadqueue_push(ThreadQueue *queue, void * callback) {
    pthread_mutex_lock(&queue->mutex);
    threadqueue_xpush(queue, callback);
    pthread_mutex_unlock(&queue->mutex);

    syscall(SYS_futex, &queue->length, FUTEX_WAKE, 1, NULL);
}

static void threadqueue_wakeall(ThreadQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    threadqueue_xpush(queue, NULL);
    pthread_mutex_unlock(&queue->mutex);

    syscall(SYS_futex, &queue->length, FUTEX_WAKE, INT32_MAX, NULL);
}

static void threadqueue_free(ThreadQueue *queue) {

    pthread_mutex_destroy(&queue->mutex);

    queue->length = 0;
    queue->capacity = 0;

    free(queue->queue);
}

#endif //KVXHTTP_QUEUE_H
