#ifndef READBAM_H
#define READBAM_H

#include <stdlib.h>
#include "uthash.h"

#define MAX_READ_ID_LENGTH 100

// Structure for storing reads associated with a given reads_id
typedef struct {
    char reads_id[MAX_READ_ID_LENGTH]; // Key: reads_id
    char **lines;                      // Value: Array of lines
    size_t count;                      // Number of lines
    size_t capacity;                   // Capacity of the array
    UT_hash_handle hh;                 // UTHash handle
} ReadsHash;

// Function declarations
void add_line_to_reads(ReadsHash **hash, const char *reads_id, const char *line);
void free_reads_hash(ReadsHash *hash);

#endif
