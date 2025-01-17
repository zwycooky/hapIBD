#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h" // Include uthash for hash table implementation
#include "readbam.h"

void add_line_to_reads(ReadsHash **hash, const char *reads_id, const char *line) {
    ReadsHash *entry;

    // Check if reads_id exists in the hash table
    HASH_FIND_STR(*hash, reads_id, entry);
    if (!entry) {
        // If not found, create a new entry
        entry = (ReadsHash *)malloc(sizeof(ReadsHash));
        strncpy(entry->reads_id, reads_id, MAX_READ_ID_LENGTH);
        entry->lines = malloc(10 * sizeof(char *)); // Start with a small capacity
        entry->count = 0;
        entry->capacity = 10;
        HASH_ADD_STR(*hash, reads_id, entry);
    }

    // Expand the array if needed
    if (entry->count >= entry->capacity) {
        entry->capacity *= 2;
        entry->lines = realloc(entry->lines, entry->capacity * sizeof(char *));
        if (!entry->lines) {
            fprintf(stderr, "Error: Memory reallocation failed\n");
            exit(1);
        }
    }

    // Add the line to the array
    entry->lines[entry->count] = strdup(line); // Duplicate the line
    entry->count++;
}

void free_reads_hash(ReadsHash *hash) {
    ReadsHash *current, *tmp;

    HASH_ITER(hh, hash, current, tmp) {
        // Free each entry's lines
        for (size_t i = 0; i < current->count; i++) {
            free(current->lines[i]);
        }
        free(current->lines);

        // Free the hash table entry
        HASH_DEL(hash, current);
        free(current);
    }
}
