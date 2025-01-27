#include "analyze_hap.h"

// Process and write results based on haplotype counts and conditions
void process_blocks(char ***output_array, int *output_size, BlockCount *hap_block_count, WindowBlock *window_blocks,
                  int block_read_nums, int chr_num) {
    BlockCount *entry, *tmp;
    int line_index = 0;
    int initial_size = 10; // 初始分配的行数
    *output_array = (char **)malloc(initial_size * sizeof(char *));
    if (*output_array == NULL) {
        perror("Failed to allocate memory for output_array");
        return;
    }
    *output_size = 0;

    HASH_ITER(hh, hap_block_count, entry, tmp) {
        int total_reads = entry->hap1 + entry->hap2 + entry->low_accuracy;
        if (total_reads == 0) {
		continue;
	}
	double ph1_ratio = entry->hap1 / (double)total_reads;
        double ph2_ratio = entry->hap2 / (double)total_reads;
        double ph3_ratio = entry->low_accuracy / (double)total_reads;
	
        WindowBlock *window_entry;
        HASH_FIND_INT(window_blocks, &entry->block_id, window_entry);
	
	//printf("%d\n", entry->block_id);

        if (window_entry) {
            int start = window_entry->start;
            int end = window_entry->end;
            char line[MAX_LINE_LENGTH];
            if (entry->hap1 >= block_read_nums && entry->hap2 >= block_read_nums &&
                ph1_ratio >= 0.4 && ph2_ratio >= 0.4) {
                // 使用 sprintf 将格式化字符串存储到 line 中
                sprintf(line, "%d\t%d\t%d\t%d\t%d\t%d\t2", chr_num, start, end, entry->hap1, entry->hap2, entry->low_accuracy);
            } else if (entry->hap1 >= block_read_nums && ph1_ratio >= 0.4 && ph3_ratio >= 0.4) {
                sprintf(line, "%d\t%d\t%d\t%d\t%d\t%d\t1", chr_num, start, end, entry->hap1, entry->hap2, entry->low_accuracy);
            } else if (entry->hap2 >= block_read_nums && ph2_ratio >= 0.4 && ph3_ratio >= 0.4) {
                sprintf(line, "%d\t%d\t%d\t%d\t%d\t%d\t1", chr_num, start, end, entry->hap1, entry->hap2, entry->low_accuracy);
            } else if (entry->low_accuracy >= block_read_nums && ph3_ratio >= 0.8) {
                sprintf(line, "%d\t%d\t%d\t%d\t%d\t%d\t0", chr_num, start, end, entry->hap1, entry->hap2, entry->low_accuracy);
	    } else if (entry->hap1 >= block_read_nums && (ph1_ratio >= 0.8 || ph2_ratio >= 0.8)) {
            	sprintf(line, "%d\t%d\t%d\t%d\t%d\t%d\t1", chr_num, start, end, entry->hap1, entry->hap2, entry->low_accuracy);
	    } else {
		continue;
	    	printf("%s\n",line);
	    }
            // 将生成的 line 存储到 output_array 中
            (*output_array)[line_index] = (char *)malloc((strlen(line) + 1) * sizeof(char));
            if ((*output_array)[line_index] == NULL) {
               	perror("Failed to allocate memory for line");
               	return;
            }
            strcpy((*output_array)[line_index], line);
            line_index++;
            (*output_size)++;
	
            // 检查是否需要扩容
            if (line_index >= initial_size) {
               	initial_size *= 2;
               	char **temp = (char **)realloc(*output_array, initial_size * sizeof(char *));
               	if (temp == NULL) {
                   	perror("Failed to reallocate memory for output_array");
                    	return;
                }
                *output_array = temp;
            }
	}
    }
}

void freeOutput(char **data, int num_lines) {
    if (data == NULL) {
        return;
    }
    for (int i = 0; i < num_lines; i++) {
        if (data[i]!= NULL) {
            free(data[i]);
        }
    }
    free(data);
}
