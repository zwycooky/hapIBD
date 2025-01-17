#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "phasing_reads.h"
#include "get_s_e_pos.h"
#include "binarySearch.h"
#include "get_base_in_target.h"
#include "max_block.h"

typedef struct {
    int key;        // 键 (数字)
    int value;      // 值
    UT_hash_handle hh; // uthash 的句柄
} HashEntry;

void *phasing_reads(char **tmp, char **tmp_hap, int *tmp_hap_pos, int reads_count, int hap_count, int snp_nums_for_reads, PhasingRes *result) {
    int reads_len = 0, total_count = 0, snp1_count = 0, snp2_count = 0, reads_start = 0, reads_end = 0;
	
	// start hash 
	HashTableEntry *hash_table = NULL;
	
	char reads_id[100];

    for (int i = 0; i < reads_count; i++) {
        char *line = tmp[i];
        char cigar[MAX_CIGAR_LENGTH];
	char seq[MAX_READ_LENGTH];

        // Parse the input line
        //sscanf(line, "%s\t%*s\t%*s\t%d\t%*s\t%s\t%*s\t%*s\t%*s\t%s", reads_id, &reads_start, cigar, seq);
	char *fields[100];  // 假设最多有 10 个字段
	int j = 0;

	char *ptr = line;
	fields[j++] = ptr;  // 第一个字段起始地址

	while ((ptr = strchr(ptr, '\t')) != NULL) {
    		*ptr = '\0';   // 替换 '\t' 为 '\0'，分隔字段
    		fields[j++] = ++ptr;  // 下一个字段起始地址
	}

	// 提取需要的字段
	strcpy(reads_id, fields[0]);         // 第一个字段
	reads_start = atoi(fields[3]);       // 第四个字段
	strcpy(cigar, fields[5]);            // 第六个字段
	strcpy(seq, fields[9]);              // 第十个字段

	//printf("%s\n",reads_id);
        // Get reads len
        
	reads_len = strlen(seq);

        // Get start and end positions
        char **cigar_ops;
	cigar_ops = (char **)malloc(150 * sizeof(char *));
        int cigar_count = get_s_e_pos(reads_start, cigar, cigar_ops, &reads_end);
	//printf("%s\n", cigar_ops[0]);

        // Filter hap records based on position
        int search_s = binarySearch(reads_start, tmp_hap_pos, hap_count);
        int search_e = binarySearch(reads_end, tmp_hap_pos, hap_count);	
	
	//printf("%s\n",reads_id);
	
        // Process filtered hap records
        for (int j = search_s; j < search_e; j++) {
		char *tmp_hap_line = tmp_hap[j];
		char snp1[2], snp2[2];
		int pos, block_id;
		//printf("%s\n",tmp_hap_line);
		//sscanf(tmp_hap_line, "%d %s %s %d", &pos, snp1, snp2, &block_id);
            	char *ptr = tmp_hap_line;
		char *end;

		// 解析第一个整数
		pos = strtol(ptr, &end, 10);
		ptr = end + 1;  // 跳过空格

		// 解析第一个字符串
		end = strchr(ptr, '\t');
		strncpy(snp1, ptr, end - ptr);
		snp1[end - ptr] = '\0';  // 添加字符串结束符
		ptr = end + 1;

		// 解析第二个字符串
		end = strchr(ptr, '\t');
		strncpy(snp2, ptr, end - ptr);
		snp2[end - ptr] = '\0';
		ptr = end + 1;

		// 解析第二个整数
		block_id = strtol(ptr, NULL, 10);

		//printf("%d\t%d\t%d\n",pos, reads_start, reads_end);
		//printf("%s\t%d\t%d\t%d\n",reads_id,pos,reads_start,reads_end);

		if (pos >= reads_start && pos <= reads_end) {
			// Compute overlap
			//printf("%s\n", reads_id);
			int relative_pos = pos - reads_start + 1;
			
			int target_base_num = get_base_in_target(relative_pos, cigar_ops, cigar_count);
			
			if (target_base_num == -1) continue;
			
			char target_base = seq[target_base_num - 1];

			char temp_target_base[2];
			temp_target_base[0] = target_base;
			temp_target_base[1] = '\0';  // 添加字符串结束标志

			if (strcmp(temp_target_base, snp1) == 0) {
				snp1_count++;
			} else if (strcmp(temp_target_base, snp2) == 0) {
				snp2_count++;
			}
			total_count++;
			
			// insert to hash to find max_block
			add_to_hash(&hash_table, block_id);
		}
		//printf("%s\t%d\n", reads_id,2);
	}
	
	//printf("%s\t%d\n",reads_id,3);

	free_cigar_ops(cigar_ops, cigar_count);
    
    }
	
	// Find the key with the maximum value
    result->max_block_id = get_max_key(hash_table);
	// Free the hash table
    free_hash_table(&hash_table);
	
    // Compute accuracy and return result
    //char *result = malloc(256);
    if (total_count >= snp_nums_for_reads) {
        if (snp1_count > snp2_count) {
            //double accuracy = (double)snp1_count / total_count * 100.0;
            //sprintf(result, "1\t%.2f\t%d\t%d\t%d", accuracy, reads_start, reads_end, max_block_id);
            result->phasedhap = 1;
	    result->accuracy = (double)snp1_count / total_count * 100.0;
	} else if (snp1_count < snp2_count) {
            //double accuracy = (double)snp2_count / total_count * 100.0;
            //sprintf(result, "2\t%.2f\t%d\t%d\t%d", accuracy, reads_start, reads_end, max_block_id);
	    result->phasedhap = 2;
	    result->accuracy = (double)snp2_count / total_count * 100.0;
        } else {
	    result->phasedhap = 0;	
	    result->accuracy = 0;
	    result->max_block_id = -1;
            //sprintf(result, "0\t0\t%d\t%d\t%d", reads_start, reads_end, -1);
        }
    } else {
	result->phasedhap = 0;    
	result->accuracy = 0;
	result->max_block_id = -1;
        //sprintf(result, "0\t0\t%d\t%d\t%d", reads_start, reads_end, -1);
    }
    
    //printf("%s\n%s\n",reads_id,result);

    //return result;
}
