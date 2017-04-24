#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

void setHandler(int signal, void (*handler)(int))
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;

    if (sigaction(signal, &action, NULL) < 0)
        ERR("sigaction");
}

int makeSocket(int domain, int type)
{
    int socketDes = socket(domain, type, 0);
    if (socketDes < 0)
        ERR("socket");
    return socketDes;
}

void bindSocket(int serverSocket, int16_t port)
{
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        ERR("bind");
}

void bindSocketAndListen(int serverSocket, int16_t port, int backlog)
{
    bindSocket(serverSocket, port);
    if (listen(serverSocket, backlog) < 0)
        ERR("listen");
}

sigset_t prepareBlockMask()
{
    sigset_t blockMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGINT);
    sigaddset(&blockMask, SIGPIPE);
    return blockMask;
}
