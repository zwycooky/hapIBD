#include "read_haphash.h"

// 创建字符串数组
StringArray *create_hap_string_array() {
    StringArray *str_array = (StringArray *)malloc(sizeof(StringArray));
    str_array->size = 0;
    str_array->capacity = INITIAL_ARRAY_SIZE;
    str_array->array = (char **)malloc(str_array->capacity * sizeof(char *));
    return str_array;
}

// 添加字符串到字符串数组
void add_to_hap_string_array(StringArray *str_array, const char *str) {
    // 检查是否需要扩容
    if (str_array->size == str_array->capacity) {
        str_array->capacity *= 2;
        str_array->array = (char **)realloc(str_array->array, str_array->capacity * sizeof(char *));
    }
    str_array->array[str_array->size] = strdup(str); // 复制字符串
    str_array->size++;
}

// 创建并初始化哈希表
HashNode *create_hap_hash_table() {
    return NULL; // uthash 使用 NULL 初始化哈希表
}

void add_to_pos_hash_table(HashNodePos **hash_table, const char *key, int value) {
    HashNodePos *node;

    // 检查是否已有相同的 key
    HASH_FIND_STR(*hash_table, key, node);
    if (node == NULL) {
        // 如果 key 不存在，创建新节点
        node = (HashNodePos *)malloc(sizeof(HashNodePos));
        if (node == NULL) {
            perror("malloc failed");
            return;
        }
        node->key = strdup(key); // 复制 key
        if (node->key == NULL) {
            perror("strdup failed");
            free(node);
            return;
        }

        // 初始化动态数组
        node->values = (int *)malloc(sizeof(int));
        if (node->values == NULL) {
            perror("malloc failed");
            free(node->key);
            free(node);
            return;
        }
        node->values[0] = value; // 插入第一个值
        node->size = 1;

        // 添加新节点到哈希表
        HASH_ADD_KEYPTR(hh, *hash_table, node->key, strlen(node->key), node);
    } else {
        // 如果 key 已存在，扩展数组并添加新值
        node->values = (int *)realloc(node->values, (node->size + 1) * sizeof(int));
        if (node->values == NULL) {
            perror("realloc failed");
            return;
        }
        node->values[node->size] = value; // 添加新值
        node->size++;
    }
}

// 插入键值对
void insert(HashNode **table, const char *key, const char *value) {
    HashNode *node;
    // 查找键是否存在
    HASH_FIND_STR(*table, key, node);
    if (node == NULL) {
        // 键不存在，创建新节点
        node = (HashNode *)malloc(sizeof(HashNode));
        node->key = strdup(key);
        node->value = create_hap_string_array();
        HASH_ADD_KEYPTR(hh, *table, node->key, strlen(node->key), node);
    }
    // 添加值到字符串数组
    add_to_hap_string_array(node->value, value);
}

// 查找键对应的字符串数组
StringArray *search(HashNode *table, const char *key) {
    HashNode *node;
    HASH_FIND_STR(table, key, node);
    return node ? node->value : NULL;
}

HashNodePos *search_pos(HashNodePos *hash_table, const char *key) {
    HashNodePos *node = NULL;
    HASH_FIND_STR(hash_table, key, node); // 使用 uthash 查找
    return node; // 返回找到的节点，或者 NULL
}

void update_chr_max_pos(ChrMaxPos **chr_len_map, const char *chr, int pos) {
    ChrMaxPos *entry;

    // 查找当前染色体是否已经在哈希表中
    HASH_FIND_STR(*chr_len_map, chr, entry);

    if (!entry) {
        // 如果不存在，则创建新节点
        entry = (ChrMaxPos *)malloc(sizeof(ChrMaxPos));
        entry->chr = strdup(chr); // 复制染色体名称
        entry->max_pos = pos;     // 初始化最大位置信息
        HASH_ADD_KEYPTR(hh, *chr_len_map, entry->chr, strlen(entry->chr), entry);
    } else {
        // 如果存在，则更新最大位置信息
        if (pos > entry->max_pos) {
            entry->max_pos = pos;
        }
    }
}

// 读取 hap 文件并存储到哈希表中
void read_hap_file(const char *hap_file, HashNode **hap_map, ChrMaxPos **chr_len_map, HashNodePos **hap_pos_map) {
    FILE *file = fopen(hap_file, "r");
    if (!file) {
        perror("Error opening hap file");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // 移除换行符

        char chr[100], hap1[10], hap2[10];
        int pos, block;
        //int fields = sscanf(line, "%s %d %s %s %d", chr, &pos, hap1, hap2, &block);
        //if (fields < 4) {
        //    fprintf(stderr, "Malformed line: %s\n", line);
        //    continue;
        //}
	char *ptr = line;
	char *endptr;
	char *end;

	// 解析第一个字段
	end = strchr(ptr, '\t');
	strncpy(chr, ptr, end - ptr);
	chr[end - ptr] = '\0';
	ptr = end + 1;

	// 解析第二个字段
	pos = strtol(ptr, &end, 10);
	ptr = end + 1;

	// 解析第三个字段
	end = strchr(ptr, '\t');
	strncpy(hap1, ptr, end - ptr);
	hap1[end - ptr] = '\0';
	ptr = end + 1;

	// 解析第四个字段
	end = strchr(ptr, '\t');
	strncpy(hap2, ptr, end - ptr);
	hap2[end - ptr] = '\0';
	ptr = end + 1;

	// 解析第五个字段
	block = strtol(ptr, &endptr, 10);

        // 构建记录
        char record[256];
        if (ptr != endptr) {
            snprintf(record, sizeof(record), "%d\t%s\t%s\t%d", pos, hap1, hap2, block);
        } else {
            snprintf(record, sizeof(record), "%d\t%s\t%s\t%d", pos, hap1, hap2, 1);
        }

        // 查找或创建染色体的动态数组
        insert(hap_map, chr, record);
	
	add_to_pos_hash_table(hap_pos_map, chr, pos);	

        // 更新 chr_len_map
        update_chr_max_pos(chr_len_map, chr, pos);
    }

    fclose(file);
}

// 释放动态数组
void free_chr_len_map(ChrMaxPos *chr_len_map) {
    ChrMaxPos *entry, *tmp;
    HASH_ITER(hh, chr_len_map, entry, tmp) {
        HASH_DEL(chr_len_map, entry); // 从哈希表中删除
        free(entry->chr);             // 释放键内存
        free(entry);                  // 释放节点内存
    }
}

// 释放哈希表
void free_hap_hash_table(HashNode *table) {
    HashNode *node, *tmp;
    HASH_ITER(hh, table, node, tmp) {
        // 释放字符串数组
        for (int i = 0; i < node->value->size; i++) {
            free(node->value->array[i]);
        }
        free(node->value->array);
        free(node->value);
        // 删除节点
        HASH_DEL(table, node);
        free(node->key);
        free(node);
    }
}

void free_pos_hash_table(HashNodePos **hash_table) {
    HashNodePos *current_node, *tmp;
    HASH_ITER(hh, *hash_table, current_node, tmp) {
        HASH_DEL(*hash_table, current_node); // 从哈希表中删除
        free(current_node->values);         // 释放数组
        free(current_node->key);            // 释放 key
        free(current_node);                 // 释放节点
    }
}
