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

    int t = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int)) < 0)
        ERR("setsockopt");

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

ssize_t bulkRead(int fd, char *buf, size_t length)
{
    size_t totalBytes = 0;
    while (length > 0)
    {
        ssize_t bytesRead = TEMP_FAILURE_RETRY(read(fd, buf, length));
        if (bytesRead < 0)
            return bytesRead;
        if (bytesRead == 0)
            return totalBytes;

        buf += bytesRead;
        totalBytes += bytesRead;
        length -= bytesRead;
    }

    return totalBytes;
}

ssize_t bulkWrite(int fd, char *buf, size_t length)
{
    size_t totalBytes = 0;
    while (length > 0)
    {
        ssize_t bytesWritten = TEMP_FAILURE_RETRY(write(fd, buf, length));
        if (bytesWritten < 0)
            return bytesWritten;
        if (bytesWritten == 0)
            return bytesWritten;

        buf += bytesWritten;
        totalBytes += bytesWritten;
        length -= bytesWritten;
    }

    return totalBytes;
}

void sendMessage(int socketFd, char* message)
{
    //printf("Size: %d\n", (int)strlen(message));
    if (bulkWrite(socketFd, message, (int)strlen(message)) < 0) 
        ERR("write");

    printf("[SENT] '%s' to %d\n", message, socketFd);
}

void receiveMessage(int socketFd, char* message)
{
    if (recv(socketFd, message, BUFFER_SIZE-1, 0) < 0) 
        ERR("read");

    printf("[GOT] '%s' from %d\n", message, socketFd);
}