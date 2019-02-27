#ifndef queue_h
#define queue_h

typedef struct QNode
{
    unsigned value;
    struct QNode *next;
} QNode;

typedef struct Queue
{
    QNode *front, *rear;
    int length, max;
} Queue;

Queue queueConstructor(int max);
int enqueue(Queue *q, unsigned toInsert);
int dequeue(Queue *q);
void deleteQueue(Queue *q);
int existsInQueue(Queue *q, unsigned toFind);
void printQueue(Queue *q);

#endif /* queue_h */
