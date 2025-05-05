#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "binarySearch.h"
#include "globals.h"
#include <string.h>
#include "uthash.h" // Include uthash for hash table implementation
#include "readbam.h"
#include "phasing_reads.h"
#include "hap_structure.h"
#include "analyze_hap.h"

#define MAX_COMMAND_LENGTH 1024
#define MAX_SAM_LINE_LENGTH 2048
#define MAX_READ_GROUP_NUM 10

typedef struct {
    int window_id;
    char chromosome[100];
    int start;
    int end;
    int block_reads_num;
    int snps_for_reads;
    char **array;
    int *pos_array;
    int array_size;
    char bam_file[300];
} ThreadArgs;

char** slice_string_array(char *arr[], int start, int end, int *slice_length, WindowBlock **hash);
int *slice_array(int *arr, size_t start, size_t end, size_t *slice_size);
void freeData(char ***data, int num_lines);
void* process_window(void* arg);
void free_window_blocks(WindowBlock *window_blocks);
void free_hap_block_count(BlockCount *hap_block_count);
int extractLastNumber(const char *str);

#endif
