//
// Created by kvxmmu on 2/21/21.
//

#ifndef KVXHTTP_THREADPOOL_H
#define KVXHTTP_THREADPOOL_H

#include "queue.h"

#include <stdbool.h>

typedef void (*free_callback_t)(void *);

typedef struct {
    size_t workers_count;

    pthread_t *threads;
    ThreadQueue queue;

    bool polling;
} ThreadPool;

typedef struct {
    void *argument;

    callback_t callback;
    free_callback_t free_function;
} WorkerArgument;

static void *threadqueue_worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;

    while (1) {
        void *callback_arg = threadqueue_pop(&pool->queue, false);

        if (callback_arg == NULL) {
            break;
        }

        threadqueue_pop(&pool->queue, true);

        WorkerArgument *argument = (WorkerArgument *)callback_arg;

        argument->callback(argument->argument);
        argument->free_function(argument->argument);

        free(argument);
    }

    pthread_exit(0);
}

static ThreadPool *threadpool_create(size_t workers_count) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));

    pool->queue = threadqueue_create(DEFAULT_INITIAL_CAPACITY);
    pool->threads = calloc(workers_count, sizeof(pthread_t));
    pool->workers_count = workers_count;
    pool->polling = true;

    for (size_t id = 0; id < workers_count; id++) {
        pthread_create(&pool->threads[id], NULL,
                threadqueue_worker, pool);
    }

    return pool;
}

static void threadpool_submit(ThreadPool *pool, callback_t callback,
        void *argument) {
    WorkerArgument *arg = malloc(sizeof(WorkerArgument));

    arg->argument = argument;
    arg->free_function = free;
    arg->callback = callback;

    threadqueue_push(&pool->queue, arg);
}

static void threadpool_join_all(ThreadPool *pool) {
    int status_addr;

    for (size_t id = 0; id < pool->workers_count; id++) {
        pthread_join(pool->threads[id], (void **)&status_addr);
    }
}

static void threadpool_free(ThreadPool *pool) {
    pool->polling = false;

    threadqueue_wakeall(&pool->queue);
    threadpool_join_all(pool);

    threadqueue_free(&pool->queue);
    free(pool->threads);
    free(pool);
}

#endif //KVXHTTP_THREADPOOL_H
