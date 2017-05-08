#include "common.h"
#include "server-helpers.h"

#include <sys/select.h>
#include <semaphore.h>

#define BACKLOG 10

void acceptNewClient(serverArgs_t *serverData, int threadId)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(struct sockaddr_in);
    int clientSocket = TEMP_FAILURE_RETRY(accept(serverData->serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen));
    if (clientSocket < 0)
        ERR("accept");

    if (threadId > 0)
    {
        int freeIndex = -1;
        for(int i = 0; i< MAX_WORKERS; i++)
        {
            if(serverData->workers[i].id < 0)
            {
                freeIndex = i;
                break;
            }
        }

        if(freeIndex == -1)
        {
            ERR("NO THREAD SLOTS!");
        }
        (*serverData->clientsConnected)++;

        serverData->workers[freeIndex].id = threadId;
        serverData->workers[freeIndex].clientSocket = clientSocket;
        serverData->workers[freeIndex].clientAddr = clientAddr;

        if (pthread_create(&serverData->workers[freeIndex].tid, NULL, workerThread, (void*)&serverData->workers[freeIndex]) != 0)
            ERR("pthread_create");
    }
}

void resetThreads(int initial, serverArgs_t *serverData)
{
    // Reset common variables
    printf("[SERVER] resetting threads\n");
    *(serverData->clientsConnected) = 0;

    if (!initial && pthread_cond_destroy(serverData->cond) != 0)
        ERR("pthread_cond_destroy");

    if (pthread_cond_init(serverData->cond, NULL) != 0)
        ERR("pthread_cond_init");

    *(serverData->isCondTrue) = 0;

    // Assign 
    workerThreadArgs_t* workers = serverData->workers;
    for(int i = 0; i < MAX_WORKERS; i++)
    {
        resetThreadArgs(&workers[i]);
        workers[i].serverData = serverData;
    }
}

void serverLoop(serverArgs_t *serverData)
{
    int nextThreadId = 1;

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

        acceptNewClient(serverData, nextThreadId++);
        printf("[SERVER] client connected\n");
        printf("[SERVER] have %d clients now\n", *(serverData->clientsConnected));
    }
}

int main(int argc, char **argv)
{
    int16_t port;
    parseArguments(argc, argv, &port);

    printf("[SERVER] Initialization\n");
    workerThreadArgs_t workers[MAX_WORKERS];
    sigset_t previousMask, previousMaskWithSigPipe;
    setSignalHandling(&previousMask, &previousMaskWithSigPipe);

    serverArgs_t serverData;
    int clientsConnected = 0, isCondTrue = 0;
    pthread_cond_t cond;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    serverData.workers = workers;
    serverData.clientsConnected = &clientsConnected;
    serverData.isCondTrue = &isCondTrue;
    serverData.mutex = &mutex;
    serverData.cond = &cond;
    serverData.serverSocket = makeSocket(AF_INET, SOCK_STREAM);
    serverData.previousMask = &previousMask;
    serverData.previousMaskWithSigPipe = &previousMaskWithSigPipe;

    bindSocketAndListen(serverData.serverSocket, port, BACKLOG);

    resetThreads(1, &serverData);

    printf("[SERVER] Listening on port %d\n", port);
    serverLoop(&serverData);

    performServerCleanup(&serverData);
    printf("[SERVER] Terminated\n");
    return EXIT_SUCCESS;
}
