#include "common.h"
#include "worker-thread.h"

void *workerThread(void *args)
{
    workerThreadArgs_t *workerArgs = (workerThreadArgs_t*)args;
    sigset_t blockMask = prepareBlockMask(),
        previousMask;
    if (pthread_sigmask(SIG_BLOCK, &blockMask, &previousMask) < 0)
        ERR("pthread_sigmask");
    printf("[%d] started\n", workerArgs->id);


    printf("[%d] terminates\n", workerArgs->id);
    free(args);
    if (pthread_sigmask(SIG_SETMASK, &previousMask, NULL) < 0)
        ERR("pthread_sigmask");
    return NULL;
}
