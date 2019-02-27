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
    q.list = listConstructor();
    
    return q;
}

int enqueue(Queue *q, unsigned toInsert)
{
    if(q->list.length < q->max)
    {
        return add(&q->list, toInsert); // true
    }
    
    return 0; // false
}

unsigned dequeue(Queue *q)
{
    return delete_front(&q->list);
}

void deleteQueue(Queue *q)
{
    deleteList(&q->list);
}

int existsInQueue(Queue *q, unsigned toFind)
{
    return existsInList(&q->list, toFind);
}

void printQueue(Queue *q)
{
    printList(&q->list);
}
