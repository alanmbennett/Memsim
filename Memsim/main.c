#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

const int ENTRIES = 1048576; // 2^32 / 2^12 = 2^20 = 1048576 possible pages
unsigned disk[ENTRIES];
unsigned dirty[ENTRIES];
unsigned clean[ENTRIES];
unsigned *RAM;
int disk_reads = 0, disk_writes = 0;

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
    RAM = malloc(nframes * sizeof(unsigned));
    
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
}

void fifo(FILE *file, int nframes, char* mode)
{
    
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
    printf("events in trace: %ld", events);
    printf("total disk reads: %ld", reads);
    printf("total disk writes: %ld", writes);
}
