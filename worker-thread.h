#ifndef WORKER_THREAD_H_INCLUDED
#define WORKER_THREAD_H_INCLUDED

#include "server-helpers.h"

typedef struct workerThreadArgs {
    int id;
    struct serverData *serverData;
} workerThreadArgs_t;

void *workerThread(void *args);

#endif
