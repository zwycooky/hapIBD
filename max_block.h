#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include "uthash.h"

// Define the hash table entry
typedef struct HashTableEntry {
    int key;                  // Use an integer key
    int value;                // Value to store
    UT_hash_handle hh;        // Hash table handle
} HashTableEntry;

// Function declarations
void add_to_hash(HashTableEntry **hash_table, int key);
int get_value(HashTableEntry *hash_table, int key);
int get_max_key(HashTableEntry *hash_table);
void free_hash_table(HashTableEntry **hash_table);

#endif
