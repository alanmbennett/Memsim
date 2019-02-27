//
//  queue.c
//  Memsim
//
//  Created by Alan Bennett on 2/27/19.
//  Copyright © 2019 Alan Bennett. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue queueConstructor(int max)
{
    Queue q;
    q.max = max;
    q.length = 0;
    q.front = q.rear = NULL;
    
    return q;
}

int enqueue(Queue *q, unsigned toInsert)
{
    if(q->length < q->max)
    {
        QNode *node = malloc(sizeof(QNode));
        node->value = toInsert;
        node->next = NULL;
        
        if(q->length == 0)
        {
            q->front = node;
            q->rear = q->front;
        }
        else
        {
            q->rear->next = node;
            q->rear = q->rear->next;
        }
        
        q->length++;
        
        return 1; // true
    }
    
    return 0; // false
}

int dequeue(Queue *q)
{
    if(q->front != NULL)
    {
        QNode *toDelete = q->front;
        q->front = q->front->next;
        free(toDelete);
        toDelete = NULL;
        q->length--;
        
        return 1; // true
    }
    
    return 0; // false
}

void deleteQueue(Queue *q)
{
    int i, amountToDelete = q->length;
    
    for(i = 0; i < amountToDelete; i++)
    {
        dequeue(q);
    }
}

int existsInQueue(Queue *q, unsigned toFind)
{
    QNode *p;
    
    for(p = q->front; p != NULL; p = p->next)
    {
        if(p->value == toFind)
            return 1; // true
    }
    
    return 0; // false
}

void printQueue(Queue *q)
{
    QNode *p;
    
    printf("[ ");
    
    for(p = q->front; p != NULL; p = p->next)
    {
        printf("%d ", p->value);
    }
    
    printf("]");
}
