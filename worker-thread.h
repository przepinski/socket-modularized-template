#ifndef WORKER_THREAD_H_INCLUDED
#define WORKER_THREAD_H_INCLUDED

#include <semaphore.h>
#include <pthread.h>
#include "client-queue.h"

typedef struct workerThreadArgs {
    int id;
    clientNode_t **clientQueue;
    pthread_mutex_t *clientQueueMutex;
    sem_t *gamesPendingSem;
} workerThreadArgs_t;

void *workerThread(void *args);

#endif
