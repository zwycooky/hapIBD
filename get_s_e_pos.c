#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "get_s_e_pos.h"

// 函数：分割CIGAR字符串并计算起始和结束位置，返回处理后的CIGAR操作数组
int get_s_e_pos(int start_pos, const char *cigar, char **cigar_ops, int *end_pos) {
    int total_len = 0;
    int num = 0;
    int cigar_idx = 0;
    int cigar_len = strlen(cigar);
    
    // 遍历 CIGAR 字符串
    for (int i = 0; i < cigar_len; i++) {
        if (isdigit(cigar[i])) {
            // 如果是数字，构建数字
            num = num * 10 + (cigar[i] - '0');
        } else {
            // 判断操作类型（M, D, N）
            if (cigar[i] == 'M' || cigar[i] == 'D' || cigar[i] == 'N') {
		// 将操作和数字组成的字符串添加到 CIGAR 操作数组中
                char *operation = (char *)malloc(20 * sizeof(char));  // 为操作分配内存
		if (operation == NULL) {
        		perror("malloc failed");
        		return 1;
    		}
                snprintf(operation, 20, "%d%c", num, cigar[i]);
		cigar_ops[cigar_idx++] = operation;  // 存入操作数组
                total_len += num;  // 累加总长度
                num = 0;  // 重置数字
            }else{
	    	char *operation = (char *)malloc(20 * sizeof(char));  // 为操作分配内存
                snprintf(operation, 20, "%d%c", num, cigar[i]);
                cigar_ops[cigar_idx++] = operation;
		num = 0;
	    }
        }
    }
    *end_pos = start_pos + total_len - 1;  // 计算终止位置
    return cigar_idx;  // 返回CIGAR操作数的数量
}

void free_cigar_ops(char **cigar_ops, int count) {
    for (int i = 0; i < count; i++) {
        free(cigar_ops[i]);
    }
    free(cigar_ops);
}
