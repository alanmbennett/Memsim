//
//  List.c
//  Memsim
//
//  Created by Alan Bennett on 2/27/19.
//  Copyright © 2019 Alan Bennett. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

List listConstructor(void)
{
    List l;
    l.front = l.rear = NULL;
    l.length = 0;

    return l;
}

int add(List *l, unsigned toInsert)
{
    LNode *node = malloc(sizeof(LNode));
    node->value = toInsert;
    node->next = NULL;
    
    if(l->length == 0)
    {
        l->front = node;
        l->rear = l->front;
    }
    else
    {
        l->rear->next = node;
        l->rear = l->rear->next;
    }
    
    l->length++;

    return 1;
}

void deleteByVal(List *l, unsigned toDelete)
{
    LNode *p, *temp;

    if(l->front == NULL)
    {
        return;
    }
    else if(l->front->value == toDelete)
    {
        temp = l->front;
        l->front = l->front->next;
        free(temp);
        l->length--;
    }
    else
    {
        for(p = l->front, temp= l->front->next; temp != NULL; p = p->next, temp = temp->next)
        {
            if(temp->value == toDelete)
                break; // true
        }
        
        if(l->rear == temp)
            l->rear = p;
        p->next = temp->next;
        free(temp);
        l->length--;
    }
}

unsigned delete_front(List *l)
{
    if(l->front != NULL)
    {
        unsigned value = l->front->value;
        LNode *toDelete = l->front;
        l->front = l->front->next;
        free(toDelete);
        toDelete = NULL;
        l->length--;
        
        return value;
    }
    
    return -1;
}

int existsInList(List *l, unsigned toFind)
{
    LNode *p;
    
    for(p = l->front; p != NULL; p = p->next)
    {
        if(p->value == toFind)
            return 1; // true
    }
    
    return 0; // false
}

void deleteList(List *l)
{
    int i, amountToDelete = l->length;
    
    for(i = 0; i < amountToDelete; i++)
    {
        delete_front(l);
    }
}

void printList(List *l)
{
    LNode *p;
    
    printf("[ ");
    
    for(p = l->front; p != NULL; p = p->next)
    {
        printf("%d ", p->value);
    }
    
    printf("]");
}
