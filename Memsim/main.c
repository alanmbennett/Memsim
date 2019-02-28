#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "queue.h"

/* Function prototypes */
void lru(FILE *file, int nframes, char* mode);
void fifo(FILE *file, int nframes, char* mode);
void vms(FILE *file, int nframes, char* mode);
void performVms(Queue *q, unsigned addr, List *clean, List *dirty, List *write, List *memory, int nframes, int* reads, int* writes);
unsigned first_out(List *clean, List*dirty, List*write, int* writes);
unsigned extractPageNo(unsigned hex);
int startsWithThree(unsigned hex);
void printFinalStats(int frames, long int events, long int reads, long int writes);

// 4 KB pages => 2^2 * 2^10 = 2^12 = 12-bit offset
// 32-bit memory addresses
// 32 - 12 = 20-bit page #
// 2^32 / 2^12 = 2^20 entries in page table

int main(int argc, const char * argv[])
{
    /* Variables */
    const int ARGS_TOTAL = 4;
    const int MAX_STR_LENGTH = 256;
    FILE *fp = NULL;
    char traceFile[MAX_STR_LENGTH];
    char algorithm[MAX_STR_LENGTH];
    char mode[MAX_STR_LENGTH];
    int nframes = 0;
    
    /* Command-line argument error-checking */
    if(argc != ARGS_TOTAL + 1) // total # of valid arguments + program name
    {
        printf("ERROR: You have only %d arguments when %d are required to run this program.\n", (argc - 1), ARGS_TOTAL);
        return EXIT_FAILURE;
    }
    
    /* Initialize variables */
    strcpy(traceFile, argv[1]);
    strcpy(algorithm, argv[3]);
    strcpy(mode, argv[4]);
    nframes = atoi(argv[2]);
    
    /* Mode error-checking */
    if(strcmp("quiet", mode) != 0)
    {
        if(strcmp("debug", mode) != 0)
        {
            printf("ERROR: \"%s\" is not a valid mode. Please use \"quiet\" or \"debug\"\n", mode);
            return EXIT_FAILURE;
        }
    }
    
    /* File error-checking */
    fp = fopen(traceFile, "r");
    
    if(fp == NULL)
    {
        printf("ERROR: The file \"%s\" does not exist. Please enter in a valid filename.\n", traceFile);
        return EXIT_FAILURE;
    }
    
    /* Execute algorithm on specified file */
    if(strcmp("lru", algorithm) == 0)
        lru(fp, nframes, mode);
    else if(strcmp("fifo", algorithm) == 0)
        fifo(fp, nframes, mode);
    else if(strcmp("vms", algorithm) == 0)
        vms(fp, nframes, mode);
    else
    {
        printf("ERROR: \"%s\" is not a valid algorithm. Please use \"lru\", \"fifo\", or \"vms\".\n", algorithm);
        return EXIT_FAILURE;
    }
    
    fclose(fp);
    
    return EXIT_SUCCESS;
}

void lru(FILE *file, int nframes, char* mode)
{
    List LRUQueue = listConstructor();
    List dirty = listConstructor();
    int disk_reads = 0, disk_writes = 0, events = 0;
    unsigned addr;
    char rw;
    
    while (fscanf(file, "%x %c", &addr, &rw) != EOF)
    {
        addr = extractPageNo(addr);
        
        if(!existsInList(&LRUQueue, addr))
        {
            disk_reads++;
            
            if(LRUQueue.length == nframes)
            {
                unsigned replaced = delete_front(&LRUQueue);
                add(&LRUQueue, addr);
                
                if(existsInList(&dirty, replaced))
                {
                    disk_writes++;
                    deleteByVal(&dirty, replaced);
                }
            }
            else
            {
                add(&LRUQueue, addr);
            }
        }
        else // exists in the list
        {
            /* Now is the most recently used so delete from queue and insert in the back position */
            deleteByVal(&LRUQueue, addr);
            add(&LRUQueue, addr);
        }
        
        if(rw == 'W')
            add(&dirty, addr);
        
        if(strcmp("debug", mode) == 0)
        {
            printList(&LRUQueue);
            printf("\n");
        }
        
        events++;
    }
    
    deleteList(&LRUQueue);
    deleteList(&dirty);
    
    printFinalStats(nframes, events, disk_reads, disk_writes);
}

void fifo(FILE *file, int nframes, char* mode)
{
    Queue q = queueConstructor(nframes);
    List dirty = listConstructor();
    int disk_reads = 0, disk_writes = 0, events = 0;
    unsigned addr;
    char rw;
    
    while (fscanf(file, "%x %c", &addr, &rw) != EOF)
    {
        addr = extractPageNo(addr);
        
        if(!existsInQueue(&q, addr))
        {
            disk_reads++;
            
            if(!enqueue(&q, addr))
            {
                unsigned replaced = dequeue(&q);
                enqueue(&q, addr);
                
                if(existsInList(&dirty, replaced))
                {
                    disk_writes++;
                    deleteByVal(&dirty, replaced);
                }
            }
        }
        
        if(rw == 'W')
            add(&dirty, addr);
        
        if(strcmp("debug", mode) == 0)
        {
            printQueue(&q);
            printf("\n");
        }
        
        events++;
    }
    
    deleteQueue(&q);
    deleteList(&dirty);
    
    printFinalStats(nframes, events, disk_reads, disk_writes);
}

void vms(FILE *file, int nframes, char* mode)
{
    int RSS = nframes/2;
    Queue qA = queueConstructor(RSS); // for process A
    Queue qB = queueConstructor(RSS); // for process B
    List clean = listConstructor();
    List dirty = listConstructor();
    List write = listConstructor();
    List memory = listConstructor();
    int disk_reads = 0, disk_writes = 0, events = 0;
    unsigned addr;
    char rw;
    
    while (fscanf(file, "%x %c", &addr, &rw) != EOF)
    {
        addr = extractPageNo(addr);
        
        if(rw == 'W')
            add(&write, addr);
        
        if(startsWithThree(addr)) // belongs to process A
            performVms(&qA, addr, &clean, &dirty, &write, &memory, nframes, &disk_reads, &disk_writes);
        else // belongs to process B
            performVms(&qB, addr, &clean, &dirty, &write, &memory, nframes, &disk_reads, &disk_writes);
        
        if(strcmp("debug", mode) == 0)
        {
            printf("FIFO(A): ");
            printQueue(&qA);
            printf("\n");
            printf("FIFO(B): ");
            printQueue(&qB);
            printf("\n");
            printf("Memory: ");
            printList(&memory);
            printf("\n\n");
        }
        
        events++;
    }
    
    deleteQueue(&qA);
    deleteQueue(&qB);
    deleteList(&clean);
    deleteList(&dirty);
    deleteList(&write);
    deleteList(&memory);
    
    printFinalStats(nframes, events, disk_reads, disk_writes);
}

void performVms(Queue *q, unsigned addr, List *clean, List *dirty, List *write, List *memory, int nframes, int* reads, int* writes)
{
    if(!existsInQueue(q, addr))
    {
        (*reads)++;
        if(!enqueue(q, addr))
        {
            unsigned page_out = dequeue(q);
            enqueue(q, addr);
            
            if(!existsInList(write, page_out))
            {
                add(clean, page_out);
            }
            else
                add(dirty, page_out);
        }
        
        if(existsInList(memory, addr))
        {
            if(existsInList(clean, addr))
                deleteByVal(clean, addr);
            else if(existsInList(dirty, addr))
            {
                deleteByVal(dirty, addr);
                deleteByVal(write, addr);
                (*writes)++;
            }
        }
        else
        {
            if(memory->length < nframes)
                add(memory, addr);
            else
            {
                unsigned frame_to_empty = first_out(clean, dirty, write, writes);
                deleteByVal(memory, frame_to_empty);
                add(memory, addr);
            }
        }
    }
}

unsigned first_out(List *clean, List*dirty, List*write, int* writes)
{
    if(clean->length > 0)
        return delete_front(clean);
    else
    {
        (*writes)++;
        unsigned val = delete_front(dirty);
        deleteByVal(write, val);
        return val;
    }
}

unsigned extractPageNo(unsigned hex)
{
    return (hex & 0xfffff000) / 0x1000;
}

int startsWithThree(unsigned hex)
{
    unsigned number = (hex & 0xf0000) / 0x10000;
    
    if(number == 0x3)
        return 1; // true
    
    return 0; // false
}

void printFinalStats(int frames, long int events, long int reads, long int writes)
{
    printf("total memory frames: %d\n", frames);
    printf("events in trace: %ld\n", events);
    printf("total disk reads: %ld\n", reads);
    printf("total disk writes: %ld\n\n", writes);
}
