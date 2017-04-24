#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define ERR(source) (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), \
                    perror(source), exit(EXIT_FAILURE) )

void setHandler(int signal, void (*handler)(int));
int makeSocket(int domain, int type);
void bindSocket(int serverSocket, int16_t port);
void bindSocketAndListen(int serverSocket, int16_t port, int backlog);
sigset_t prepareBlockMask();

#endif
