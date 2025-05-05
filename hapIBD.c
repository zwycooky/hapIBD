#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arguments.h"
#include "read_haphash.h"
#include "sliding_window.h"
#include "process_threads.h"
#include "globals.h"
#include "utils.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 互斥锁
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   // 条件变量

int active_threads = 0; // 当前活动线程数

#define MAX_LENGTH 100
#define MAX_ELEMENTS 100

// 自定义比较函数
int compare(const void *a, const void *b) {
    char *const *str_a = (char *const *)a;
    char *const *str_b = (char *const *)b;
    int num1_a, num2_a, num1_b, num2_b;
    char *token_a, *token_b;

    // 复制字符串以避免修改原始字符串
    char str_a_copy[MAX_LENGTH];
    char str_b_copy[MAX_LENGTH];
    strcpy(str_a_copy, *str_a);
    strcpy(str_b_copy, *str_b);

    // 解析第一列
    token_a = strtok(str_a_copy, "\t");
    num1_a = atoi(token_a);
    token_b = strtok(str_b_copy, "\t");
    num1_b = atoi(token_b);

    // 比较第一列
    if (num1_a < num1_b) {
        return -1;
    } else if (num1_a > num1_b) {
        return 1;
    } else {
        // 解析第二列
        token_a = strtok(NULL, "\t");
        num2_a = atoi(token_a);
        token_b = strtok(NULL, "\t");
        num2_b = atoi(token_b);
        // 比较第二列
        if (num2_a < num2_b) {
            return -1;
        } else if (num2_a > num2_b) {
            return 1;
        } else {
            return 0;
        }
    }
}

int main(int argc, char *argv[]) {
    // 定义参数结构体
    ProgramArguments args;

    // 解析命令行参数
    parse_arguments(argc, argv, &args);

    // 打印解析的参数
    printf("Parsed arguments:\n");
    printf("  BAM file: %s\n", args.bam_file);
    printf("  HAP file: %s\n", args.hap_file);
    printf("  Output prefix: %s\n", args.output_prefix);
    printf("  CPU cores: %d\n", args.cpu);
    printf("  Window length: %d\n", args.window_length);
    printf("  Step length: %d\n", args.step_length);
    printf("  Block read numbers: %d\n", args.block_read_nums);
    printf("  SNPs for reads: %d\n", args.snps_for_reads);
    
    const char *hap_file = args.hap_file;
    HashNode *hap_map = NULL; // 哈希表存储 hap 数据
    HashNodePos *hap_pos_map = NULL; // store hap pos only
    ChrMaxPos *chr_len_map = NULL; // 哈希表存储染色体长度
    
    // 读取 hap 文件
    read_hap_file(hap_file, &hap_map, &chr_len_map, &hap_pos_map);
    
    // set final merged array //
    int init_total_size = 1000;
    int total_size = 0;
    int index = 0;
    char **merged_array = (char **)malloc(init_total_size * sizeof(char *));

    // 打印 chr_len_map 内容
    printf("\nChromosome Lengths:\n");
    ChrMaxPos *chr_entry, *chr_tmp;
    HASH_ITER(hh, chr_len_map, chr_entry, chr_tmp) {
    	printf("Chromosome: %s, Max Position: %d\n", chr_entry->chr, chr_entry->max_pos);
		const char *tmp_chr = chr_entry->chr;
		int chr_end = chr_entry->max_pos;
		int win_start = 1;
		int win_end = win_start + args.window_length;
		int step = args.step_length;
		SlidingWindow *sw = generate_sliding_window(chr_end, win_start, win_end, step);

		StringArray *result = search(hap_map, tmp_chr);
		HashNodePos *pos_result = search_pos(hap_pos_map, tmp_chr);
		char **tmp_hap = result->array;
		int hap_size = result->size;
		
		printf("Sliding windows for %s:\n", tmp_chr);
		
		// start multi-threads
		pthread_t threads[sw->size];
		ThreadResult *chr_hap_results[sw->size];

		for (int i = 0; i < sw->size; i++) {
			int tmp_win_s, tmp_win_e;
			sscanf(sw->windows[i], "%d %d", &tmp_win_s, &tmp_win_e);
		
			// assign args for processing threads
			ThreadArgs* Targs = (ThreadArgs*)malloc(sizeof(ThreadArgs));
			Targs->window_id = i;
			strncpy(Targs->chromosome, tmp_chr, sizeof(Targs->chromosome) - 1);
			Targs->start = tmp_win_s;
			Targs->end = tmp_win_e;
			Targs->block_reads_num = args.block_read_nums;
			Targs->snps_for_reads = args.snps_for_reads;
			Targs->array = tmp_hap;
			Targs->array_size = hap_size;
			Targs->pos_array = pos_result->values;
			strncpy(Targs->bam_file, args.bam_file, sizeof(Targs->bam_file) - 1);
			
			pthread_mutex_lock(&mutex);
			
			// 等待有空余线程
			while (active_threads >= args.cpu) {
				pthread_cond_wait(&cond, &mutex);
			}
			
			// 创建新线程
			if (pthread_create(&threads[i], NULL, process_window, Targs) != 0) {
				perror("Failed to create thread");
				free(Targs); // 确保失败时释放内存
				pthread_mutex_unlock(&mutex);
				continue;
			}

			active_threads++; // 更新活动线程数
			pthread_mutex_unlock(&mutex);
		}
		
		// 等待所有线程完成
		for (int i = 0; i < sw->size; i++) {
			pthread_join(threads[i], (void **)&chr_hap_results[i]);
		}

		printf("All windows processed.\n");

		// 销毁锁和条件变量
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
		
		// 合并结果
                for (int i = 0; i < sw->size; i++) {
                	total_size += chr_hap_results[i]->size;
    		}
		// 检查是否需要扩容
		if (total_size >= init_total_size) {
    			init_total_size = total_size + 1000;
    			char **new_merged_array = (char **)realloc(merged_array, init_total_size * sizeof(char *));
    			if (new_merged_array == NULL) {
        			perror("realloc failed");
        			exit(1);
    			}
    			merged_array = new_merged_array;
		}

    		for (int i = 0; i < sw->size; i++) {
			//printf("size for one thread: %d\n", chr_hap_results[i]->size);
        		for (int j = 0; j < chr_hap_results[i]->size; j++) {
				//printf("%s\n", chr_hap_results[i]->array[j]);
            			merged_array[index++] = strdup(chr_hap_results[i]->array[j]);
        		}
    		}
		
		//printf("add\n");
		
		//free mem
    		for (int i = 0; i < sw->size; i++) {
			for (int j=0; j< chr_hap_results[i]->size; j++) {
				free(chr_hap_results[i]->array[j]);
			}
			free(chr_hap_results[i]->array);
    			free(chr_hap_results[i]);
		}

    		free_sliding_window(sw);

    }
    
    //printf("start sort merged_array\n");
    //qsort(merged_array, index, sizeof(char *), compare);
    //printf("sorted\n");
    
    // output IBD file //
    char IBDout[100];
    snprintf(IBDout, sizeof(IBDout), "%s%s", args.output_prefix, ".hapIBD.txt");

    FILE *IBD_file = fopen(IBDout, "w");
    if (IBD_file == NULL) {
        perror("Error opening IBDout file");
        return 0;
    }
    for (int i=0; i<index; i++) {
    	fprintf(IBD_file,"%s\n",merged_array[i]);
	free(merged_array[i]);
    }
    fclose(IBD_file);

    // 释放参数结构体中的内存
    free(merged_array);
    free_arguments(&args);
    free_hap_hash_table(hap_map);
    free_pos_hash_table(&hap_pos_map);
    free_chr_len_map(chr_len_map);    

    return EXIT_SUCCESS;
}

