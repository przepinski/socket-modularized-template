#include "common.h"
#include "client-queue.h"

void addClientToQueue(clientNode_t **head, int clientSocket)
{
    clientNode_t *newNode = malloc(sizeof(clientNode_t));
    if (newNode == NULL)
        ERR("malloc");

    newNode->clientSocket = clientSocket;
    newNode->next = *head;
    *head = newNode;
}

clientNode_t *popClientFromQueue(clientNode_t **head)
{
    if (*head == NULL)
        return NULL;

    if ((*head)->next == NULL)
    {
        clientNode_t *nodeToReturn = *head;
        *head = NULL;
        return nodeToReturn;
    }

    clientNode_t *tail = *head,
        *prev = NULL;
    while (tail->next != NULL)
    {
        prev = tail;
        tail = tail->next;
    }

    prev->next = NULL;
    return tail;
}

void clearClientQueue(clientNode_t **head)
{
    while (*head != NULL)
    {
        clientNode_t *next = (*head)->next;
        free(*head);
        *head = next;
    }
}
