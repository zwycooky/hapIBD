#include "max_block.h"
#include <stdio.h>

// Add a key to the hash table or increment its value
void add_to_hash(HashTableEntry **hash_table, int key) {
    HashTableEntry *entry;

    HASH_FIND_INT(*hash_table, &key, entry); // Find entry by numeric key
    if (entry == NULL) {
        entry = (HashTableEntry *)malloc(sizeof(HashTableEntry));
        entry->key = key;
        entry->value = 1;  // Initialize value to 1 for a new key
        HASH_ADD_INT(*hash_table, key, entry); // Add entry to the hash table
    } else {
        entry->value++; // Increment value if key already exists
    }
}

// Get the value associated with a key
int get_value(HashTableEntry *hash_table, int key) {
    HashTableEntry *entry;
    HASH_FIND_INT(hash_table, &key, entry);
    return (entry != NULL) ? entry->value : 0;
}

// Get the key with the maximum value
int get_max_key(HashTableEntry *hash_table) {
    HashTableEntry *entry, *tmp;
    int max_key = 0;
    int max_value = 0;

    HASH_ITER(hh, hash_table, entry, tmp) {
        if (entry->value > max_value) {
            max_value = entry->value;
            max_key = entry->key;
        }
    }
    return max_key;
}

// Free the hash table
void free_hash_table(HashTableEntry **hash_table) {
    HashTableEntry *entry, *tmp;
    HASH_ITER(hh, *hash_table, entry, tmp) {
        HASH_DEL(*hash_table, entry);
        free(entry);
    }
}
