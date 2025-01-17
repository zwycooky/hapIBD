#ifndef HAP_H
#define HAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uthash.h"

// 定义字符串数组的初始大小
#define INITIAL_ARRAY_SIZE 1000000

// 定义字符串数组结构
typedef struct {
    char **array;  // 字符串数组
    int size;      // 当前字符串数量
    int capacity;  // 数组容量
} StringArray;

// 哈希表节点
typedef struct {
    char *key;             // 哈希表的键
    StringArray *value;    // 哈希表的值（字符串数组）
    UT_hash_handle hh;     // 用于 uthash 的哈希句柄
} HashNode;

typedef struct {
    char *key;           // 键
    int *values;         // 动态数组，存储对应的数字
    size_t size;         // 数组大小
    UT_hash_handle hh;   // uthash 句柄
} HashNodePos;

typedef struct {
    char *chr;         // 键：染色体名称
    int max_pos;       // 值：当前的最大位置信息
    UT_hash_handle hh; // uthash 句柄
} ChrMaxPos;

// 函数声明
StringArray *search(HashNode *table, const char *key);
HashNodePos *search_pos(HashNodePos *hash_table, const char *key);
void insert(HashNode **table, const char *key, const char *value);
HashNode *create_hap_hash_table();
void add_to_hap_string_array(StringArray *str_array, const char *str);
StringArray *create_hap_string_array();

void read_hap_file(const char *hap_file, HashNode **hap_map, ChrMaxPos **chr_len_map, HashNodePos **hap_pos_map);
void free_hap_hash_table(HashNode *table);

void update_chr_max_pos(ChrMaxPos **chr_len_map, const char *chr, int pos);
void free_chr_len_map(ChrMaxPos *chr_len_map);

void add_to_pos_hash_table(HashNodePos **hash_table, const char *key, int value);
void free_pos_hash_table(HashNodePos **hash_table);

#endif // HAP_H
