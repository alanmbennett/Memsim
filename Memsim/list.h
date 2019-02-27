#ifndef list_h
#define list_h

typedef struct LNode
{
    unsigned value;
    struct LNode *next;
} LNode;

typedef struct List
{
    LNode *front, *rear;
    int length;
} List;

List listConstructor(void);
int add(List *l, unsigned toInsert);
unsigned delete_front(List *l);
void deleteByVal(List *l, unsigned toDelete);
int existsInList(List *l, unsigned toFind);
void deleteList(List *l);
void printList(List *l);

#endif
