#ifndef WORKER_THREAD_H_INCLUDED
#define WORKER_THREAD_H_INCLUDED

typedef struct workerThreadArgs 
{
    int id;
    pthread_t tid;
    int clientSocket;
    struct sockaddr_in clientAddr;

    struct serverArgs *serverData;
} workerThreadArgs_t;

void resetThreadArgs(workerThreadArgs_t *workerArgs);
void setWorkerThreadSignalHandling(sigset_t *previousMask);
void workerCleanup(void *args, sigset_t *previousMask);
void workerEmergencyCleanup(void *args);
void *workerThread(void *args);

#endif
