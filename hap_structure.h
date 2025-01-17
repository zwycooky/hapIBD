#ifndef HAP_STRUCTURE_H
#define HAP_STRUCTURE_H

#include "uthash.h"

// Define a struct for window blocks
typedef struct {
    int block_id;    // Key: block ID
    int start;       // Start position
    int end;         // End position
    UT_hash_handle hh; // Handle for uthash
} WindowBlock;

// Define a struct for block counts
typedef struct {
    int block_id;    // Key: block ID
    int hap1;        // Count for hap1
    int hap2;        // Count for hap2
    int low_accuracy; // Count for low-accuracy reads
    UT_hash_handle hh; // Handle for uthash
} BlockCount;

#endif //
