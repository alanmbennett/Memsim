#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "queue.h"

/* Function prototypes */
void lru(FILE *file, int nframes, char* mode);
void fifo(FILE *file, int nframes, char* mode);
void vms(FILE *file, int nframes, char* mode);
unsigned extractPageNo(unsigned hex);
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
    }
    
    deleteList(&LRUQueue);
    
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
    }
    
    deleteQueue(&q);
    
    printFinalStats(nframes, events, disk_reads, disk_writes);
}

void vms(FILE *file, int nframes, char* mode)
{
}

unsigned extractPageNo(unsigned hex)
{
    return (hex & 0xfffff000) / 0x1000;
}

void printFinalStats(int frames, long int events, long int reads, long int writes)
{
    printf("total memory frames: %d\n", frames);
    printf("events in trace: %ld\n", events);
    printf("total disk reads: %ld\n", reads);
    printf("total disk writes: %ld\n\n", writes);
}
