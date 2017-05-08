#ifndef SERVER_HELPERS_H_INCLUDED
#define SERVER_HELPERS_H_INCLUDED

#include "common.h"
#include "worker-thread.h"

#define PORT_MIN 1000
#define MAX_WORKERS 4

typedef struct serverArgs
{
    int serverSocket;
    workerThreadArgs_t* workers;
    int *clientsConnected;

    sigset_t *previousMask;
    sigset_t *previousMaskWithSigPipe;

    pthread_mutex_t *mutex;
    int *isCondTrue;
	pthread_cond_t *cond;
} serverArgs_t;


volatile sig_atomic_t shouldQuit;
void sigIntHandler(int signal);
void usage(char *fileName);
void parseArguments(int argc, char **argv, int16_t *port);
void setSignalHandling(sigset_t *previousMask, sigset_t *previousMaskWithSigPipe);
void performServerCleanup(serverArgs_t *serverData);


#endif
