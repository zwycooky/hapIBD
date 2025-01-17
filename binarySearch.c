#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binarySearch.h"

// 获取字段函数
void get_field(const char* str, int field, char* buffer, int buffer_size) {
    int i = 0, j = 0, tab_count = 0;

    while (str[i] != '\0') {
        if (str[i] == '\t') {
            tab_count++;
            if (tab_count > field) break;
            j = 0;
        } else if (tab_count == field) {
            if (j < buffer_size - 1) {
                buffer[j++] = str[i];
            }
        }
        i++;
    }
    buffer[j] = '\0';
}

// 字符串转整数
int str_to_int(const char* str) {
    return atoi(str);
}

// 二分查找函数
int binarySearch(int pos, int* arr, int arr_size) {
    int low = 0, high = arr_size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int mid_pos = arr[mid];

        if (mid_pos == pos) {
            return mid;
        } else if (mid_pos < pos) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return low;
}

