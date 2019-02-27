#ifndef queue_h
#define queue_h
#include "list.h"

typedef struct Queue
{
    List list;
    int max;
} Queue;

Queue queueConstructor(int max);
int enqueue(Queue *q, unsigned toInsert);
unsigned dequeue(Queue *q);
void deleteQueue(Queue *q);
int existsInQueue(Queue *q, unsigned toFind);
void printQueue(Queue *q);

#endif /* queue_h */
