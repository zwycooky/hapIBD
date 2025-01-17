#include "process_threads.h"
#include "hap_structure.h"

int *slice_array(int *arr, size_t start, size_t end, size_t *slice_size) {
    *slice_size = end - start;
    int *slice = malloc(*slice_size * sizeof(int)); // 分配内存
    if (!slice) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    memcpy(slice, arr + start, *slice_size * sizeof(int)); // 复制数据
    return slice;
}

char** slice_string_array(char *arr[], int start, int end, int *slice_length, WindowBlock **hash) {
    *slice_length = end - start; // 子数组长度
    char **slice = (char **)malloc(*slice_length * sizeof(char *));
    if (slice == NULL) {
        printf("empty hap_data in window %d-%d\n", start, end);
        return NULL;
    }

    for (int i = 0; i < *slice_length; i++) {
        slice[i] = strdup(arr[start + i]); // 复制字符串
        if (slice[i] == NULL) {
            // 处理 strdup 失败的情况
            for (int j = 0; j < i; j++) {
                free(slice[j]);
            }
            free(slice);
            return NULL;
        }

        int pos, block_id;
        // 使用更精确的 sscanf 格式
	char *ptr = arr[start + i];
	char *endp;
	char *endptr;

	// Parse the first integer (pos)
	pos = strtol(ptr, &endp, 10);
	ptr = endp + 1;  // Move to the next field

	// Skip the second and third fields
	for (int j = 0; j < 2; j++) {
    		ptr = strchr(ptr, '\t');
    		if (ptr) ptr++;
	}

	// Parse the fourth integer (block_id)
	block_id = strtol(ptr, &endptr, 10);

        //if (sscanf(arr[start + i], "%d %*s %*s %d", &pos, &block_id) == 2) {
        if (ptr != endptr) {
            WindowBlock *Wentry;		
            HASH_FIND_INT(*hash, &block_id, Wentry);
            if (!Wentry) {
                // If not found, create a new entry
                Wentry = (WindowBlock *)malloc(sizeof(WindowBlock));
                Wentry->block_id = block_id;
		Wentry->start = pos;
                Wentry->end = pos;
                HASH_ADD_INT(*hash, block_id, Wentry);
		//printf("%d\t%d\t%d\n",block_id, pos, pos);
            } else {
		//printf("%d\n",pos);
                Wentry->end = pos;
            }
        } else {
            // 处理 sscanf 失败的情况
            fprintf(stderr, "Error parsing string at index %d\n", start + i);
            for (int j = 0; j <= i; j++) {
                free(slice[j]);
            }
            free(slice);
            return NULL;
        }
    }
    return slice;
}

int extractLastNumber(const char *str) {
    int num = 0;
    sscanf(str, "%*[^0-9]%d", &num);
    return num;
}

void free_window_blocks(WindowBlock *window_blocks) {
    WindowBlock *entry, *tmp;
    HASH_ITER(hh, window_blocks, entry, tmp) {
        HASH_DEL(window_blocks, entry);
        free(entry);
    }
}

void free_hap_block_count(BlockCount *hap_block_count) {
    BlockCount *entry, *tmp;
    HASH_ITER(hh, hap_block_count, entry, tmp) {
        HASH_DEL(hap_block_count, entry);
        free(entry);
    }
}

void freeData(char ***data, int num_lines) {
    char **temp_data = *data;
    for (int i = 0; i < num_lines; i++) {
        free(temp_data[i]);
    }
    free(temp_data);
    *data = NULL;
}

void* process_window(void* arg) {
     // 将 void* 指针转换为 ThreadArgs* 指针
    ThreadArgs* args = (ThreadArgs*)arg;
    
    printf("Thread %lu: Processing window %d [%s:%d-%d]\t%d\t%d\n%s\n",
           pthread_self(), args->window_id, args->chromosome, args->start, args->end, args->block_reads_num, args->snps_for_reads, args->bam_file);
	
    // 执行任务
    // Filter hap records based on position
    int search_s = binarySearch(args->start, args->pos_array, args->array_size);
    int search_e = binarySearch(args->end, args->pos_array, args->array_size);
    
    // slice
    int hap_size = 0;
    WindowBlock *window_block_hash = NULL;
    char **tmp_hap = slice_string_array(args->array, search_s, search_e, &hap_size, &window_block_hash);
    size_t search_s_size_t = (size_t) search_s;
    size_t search_e_size_t = (size_t) search_e;
    size_t slice_size;
    int *tmp_hap_pos = slice_array(args->pos_array, search_s_size_t, search_e_size_t, &slice_size);


    //WindowBlock *window_hap_entry, *tmp2;
    //HASH_ITER(hh, window_block_hash, window_hap_entry, tmp2) {
    //   int tmpstart = window_hap_entry->start;
    //   int tmpend = window_hap_entry->end;
    //   printf("%d\t%d\t%d\n",window_hap_entry->block_id, tmpstart, tmpend);
    //}	    


    // Read bam data
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "samtools view -q 45 %s %s:%d-%d", args->bam_file, args->chromosome, args->start, args->end);
    
    // Open the samtools command for reading
    FILE *sam_file = popen(command, "r");
    if (!sam_file) {
        fprintf(stderr, "Error: Cannot open samtools command: %s\n", command);
        return NULL;
    }
    
    char line[MAX_SAM_LINE_LENGTH];
    ReadsHash *reads_hash = NULL; // Hash table for storing reads

    // Read lines from samtools output
    while (fgets(line, sizeof(line), sam_file)) {
        // Remove the newline character
        line[strcspn(line, "\n")] = 0;

        // Extract the reads_id (first column)
        char reads_id[MAX_READ_ID_LENGTH];
        if (sscanf(line, "%s", reads_id) != 1) {
            fprintf(stderr, "Error: Unable to extract reads_id from line: %s\n", line);
            continue;
        }

        // Add the line to the hash table
        add_line_to_reads(&reads_hash, reads_id, line);
    }

    pclose(sam_file);
    
    //open output
    //char hap1out[600], hap2out[600], lowaccout[600];  //
    //snprintf(hap1out, sizeof(hap1out), "%s%s", outprefix, ".hap1.txt");
    //snprintf(hap2out, sizeof(hap2out), "%s%s", outprefix, ".hap2.txt");
    //snprintf(lowaccout, sizeof(lowaccout), "%s%s", outprefix, ".low.accuracy.reads.txt");

    //FILE *phase1_file = fopen(hap1out, "w");
    //if (phase1_file == NULL) {
    //    perror("Error opening phase1 file");
    //    return (void*)1;
    //}
    //FILE *phase2_file = fopen(hap2out, "w");
    //if (phase2_file == NULL) {
    //    perror("Error opening phase2 file");
    //    return (void*)1;
    //}
    //FILE *lowacc_file = fopen(lowaccout, "w");
    //if (lowacc_file == NULL) {
    //    perror("Error opening lowacc file");
    //    return (void*)1;
    //}
    
    // start
    ReadsHash *entry, *tmp;
    BlockCount *hap_block_count = NULL;
    HASH_ITER(hh, reads_hash, entry, tmp) {
    	// store pair-end reads into tmp_reads
	char **tmp_reads = entry->lines;
	int tmp_reads_size = entry->count;
	
	//char *phasing_result;
    	int phasedhap, reads_start, reads_end, max_block_id;
        double accuracy;
	PhasingRes result;
	phasing_reads(tmp_reads, tmp_hap, tmp_hap_pos, tmp_reads_size, hap_size, args->snps_for_reads, &result);
	//printf("%s\t%d\t%d\t%d\t%d\n", args->chromosome, reads_start, reads_end, phasedhap, max_block_id);
	
	phasedhap = result.phasedhap;
	accuracy = result.accuracy;
	max_block_id = result.max_block_id;

    	//sscanf(phasing_result, "%d %f %d %d %d", &phasedhap, &accuracy, &reads_start, &reads_end, &max_block_id);
	
    	if (phasedhap > 0) {
		BlockCount *block_entry;
		HASH_FIND_INT(hap_block_count, &max_block_id, block_entry);
		if (!block_entry) {
                	block_entry = (BlockCount *)malloc(sizeof(BlockCount));
                	block_entry->block_id = max_block_id;
                	block_entry->hap1 = 0;
                	block_entry->hap2 = 0;
                	block_entry->low_accuracy = 0;
                	HASH_ADD_INT(hap_block_count, block_id, block_entry);
            	}
    		if (accuracy >= 90 && phasedhap == 1) {
			//fprintf(phase1_file, "%s\t%d\t%d\t%d\t%d\n", args->chromosome, reads_start, reads_end, phasedhap, max_block_id);
			block_entry->hap1 ++;
		}else if (accuracy >= 90 && phasedhap == 2) {
			//fprintf(phase2_file, "%s\t%d\t%d\t%d\t%d\n", args->chromosome, reads_start, reads_end, phasedhap, max_block_id);
			block_entry->hap2 ++;
		}else{
			//fprintf(lowacc_file, "%s\t%d\t%d\t%d\t%d\n", args->chromosome, reads_start, reads_end, phasedhap, max_block_id);
			block_entry->low_accuracy ++;
		}
    	}

    	//free(phasing_result);
    }
    
    // close file handles
    //fclose(phase1_file);
    //fclose(phase2_file);
    //fclose(lowacc_file);
    
    //process hap_count_block
    int chr_num = extractLastNumber(args->chromosome);
    char **output_array;
    int output_size;
    process_blocks(&output_array, &output_size, hap_block_count, window_block_hash,
                  args->block_reads_num, chr_num);
    
    ThreadResult *chr_hap_results = (ThreadResult *)malloc(sizeof(ThreadResult));
    chr_hap_results->array = output_array;
    chr_hap_results->size = output_size;
    
    // free mem
    free_reads_hash(reads_hash);
    freeData(&tmp_hap, hap_size);
    free(tmp_hap_pos);
    free_window_blocks(window_block_hash);
    free_hap_block_count(hap_block_count);
    //freeOutput(output_array, output_size);

    printf("Thread %lu: Finished window %d\n", pthread_self(), args->window_id);

    // 任务完成后更新活动线程数
    pthread_mutex_lock(&mutex);
    active_threads--;
    pthread_cond_signal(&cond); // 通知主线程有空余线程
    pthread_mutex_unlock(&mutex);

    free(arg); // 释放动态分配的内存
    return (void *)chr_hap_results;
}

