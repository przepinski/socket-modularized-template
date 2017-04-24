#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <semaphore.h>

#include "common.h"
#include "worker-list.h"
#include "client-queue.h"
#include "worker-thread.h"
#include "server-helpers.h"

#define BACKLOG 10



void serverLoop(struct serverData *serverData)
{
    // Main loop
    fd_set readFds;
    while (!shouldQuit)
    {
        FD_ZERO(&readFds);
        FD_SET(serverData->serverSocket, &readFds);
        if (pselect(serverData->serverSocket + 1, &readFds, NULL, NULL, NULL, serverData->previousMaskWithSigPipe) < 0)
        {
            if (errno == EINTR)
                continue;
            ERR("pselect");
        }

        int clientSocket = TEMP_FAILURE_RETRY(accept(serverData->serverSocket, NULL, NULL));
        if (clientSocket < 0)
            ERR("accept");

        if (pthread_mutex_lock(serverData->clientQueueMutex) != 0)
            ERR("pthread_mutex_lock");

        addClientToQueue(serverData->clientQueue, clientSocket);

        if (pthread_mutex_unlock(serverData->clientQueueMutex) != 0)
            ERR("pthread_mutex_unlock");

        printf("[SERVER] client connected\n");
    }
}

int main(int argc, char **argv)
{
    int groupSize;
    int16_t port;
    parseArguments(argc, argv, &groupSize, &port);

    printf("[SERVER] Initialization\n");
    workerThreadNode_t *workerThreadsList = NULL;
    pthread_mutex_t workerThreadsListMutex = PTHREAD_MUTEX_INITIALIZER;
    clientNode_t *clientQueue = NULL;
    pthread_mutex_t clientQueueMutex = PTHREAD_MUTEX_INITIALIZER;
    sigset_t previousMask, previousMaskWithSigPipe;
    setSignalHandling(&previousMask, &previousMaskWithSigPipe);

    struct serverData serverData;
    serverData.serverSocket = makeSocket(AF_INET, SOCK_STREAM);
    serverData.previousMask = &previousMask;
    serverData.previousMaskWithSigPipe = &previousMaskWithSigPipe;
    serverData.workerThreadsList = &workerThreadsList;
    serverData.workerThreadsListMutex = &workerThreadsListMutex;
    serverData.clientQueue = &clientQueue;
    serverData.clientQueueMutex = &clientQueueMutex;

    bindSocketAndListen(serverData.serverSocket, port, BACKLOG);

    printf("[SERVER] Listening on port %d\n", port);
    serverLoop(&serverData);

    performCleanup(&serverData);
    printf("[SERVER] Terminated\n");
    return EXIT_SUCCESS;
}
