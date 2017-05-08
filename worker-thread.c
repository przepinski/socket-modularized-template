#include "common.h"
#include "worker-thread.h"
#include "server-helpers.h"

void resetThreadArgs(workerThreadArgs_t *workerArgs)
{
    workerArgs->id = -1;
    workerArgs->tid = 0;
    workerArgs->clientSocket = -1;
}

void setWorkerThreadSignalHandling(sigset_t *previousMask)
{
    sigset_t blockMask = prepareBlockMask();
    if (pthread_sigmask(SIG_BLOCK, &blockMask, previousMask) < 0)
        ERR("pthread_sigmask");
}

void cleanup(workerThreadArgs_t *workerArgs)
{
    if (pthread_mutex_unlock(workerArgs->serverData->mutex) != 0)
        ERR("pthread_mutex_unlock");

    if (TEMP_FAILURE_RETRY(close(workerArgs->clientSocket)) < 0)
        ERR("close");

    resetThreadArgs(workerArgs);

    (*workerArgs->serverData->clientsConnected)--;
}

void workerCleanup(void *args, sigset_t *previousMask)
{
    workerThreadArgs_t *workerArgs = (workerThreadArgs_t*)args;

    cleanup(workerArgs);

    if (pthread_sigmask(SIG_SETMASK, previousMask, NULL) < 0)
        ERR("pthread_sigmask");
}

void workerEmergencyCleanup(void *args)
{
    workerThreadArgs_t *workerArgs = (workerThreadArgs_t*)args;

    cleanup(workerArgs);

    printf("[%d] emergency terminates\n", workerArgs->id);
}

void *workerThread(void *args)
{
    workerThreadArgs_t *workerArgs = (workerThreadArgs_t*)args;
    pthread_cleanup_push(workerEmergencyCleanup, args);

    sigset_t previousMask;
    setWorkerThreadSignalHandling(&previousMask);
    printf("[%d] started\n", workerArgs->id);

    if (shouldQuit)
        pthread_exit(NULL);

    // sendMessage(workerArgs->clientSocket, "Send sth to confirm");
    printf("[%d] awaiting response on %d\n", workerArgs->id, workerArgs->clientSocket);
    char receiveBuffer[BUFFER_SIZE];
    receiveMessage(workerArgs->clientSocket, receiveBuffer);
    printf("[%d] received response\n", workerArgs->id);
    // if (pthread_mutex_lock(workerArgs->serverData->mutex) != 0)
    //     ERR("pthread_mutex_lock");

    printf("[%d] terminates\n", workerArgs->id);
    workerCleanup(workerArgs, &previousMask);

    pthread_cleanup_pop(0);
    return NULL;
}
