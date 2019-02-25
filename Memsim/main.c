#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function prototypes */
void lru(FILE *traceFile, int nframes, char* mode);
void fifo(FILE *traceFile, int nframes, char* mode);
void vms(FILE *traceFile, int nframes, char* mode);
void printFinalStats(int frames, long int events, long int reads, long int writes);

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

void lru(FILE *traceFile, int nframes, char* mode)
{
    
}

void fifo(FILE *traceFile, int nframes, char* mode)
{
    
}

void vms(FILE *traceFile, int nframes, char* mode)
{
    
}

void printFinalStats(int frames, long int events, long int reads, long int writes)
{
    printf("total memory frames: %d\n", frames);
    printf("events in trace: %ld", events);
    printf("total disk reads: %ld", reads);
    printf("total disk writes: %ld", writes);
}
