#ifndef ANA_HAP_BLOCK_H
#define ANA_HAP_BLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"
#include "hap_structure.h"

#define MAX_LINE_LENGTH 200

// Function declarations
void process_blocks(char ***output_array, int *output_size, BlockCount *hap_block_count, WindowBlock *window_blocks,
                  int block_read_nums, int chr_num);
void freeOutput(char **data, int num_lines);

#endif // ANA_HAP_BLOCK_H

