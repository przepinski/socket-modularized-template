#ifndef CLIENT_QUEUE_H_INCLUDED
#define CLIENT_QUEUE_H_INCLUDED

typedef struct clientNode {
    int clientSocket;
    struct clientNode *next;
} clientNode_t;

void addClientToQueue(clientNode_t **head, int clientSocket);
clientNode_t *popClientFromQueue(clientNode_t **head);
void clearClientQueue(clientNode_t **head);

#endif
