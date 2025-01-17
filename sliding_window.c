#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sliding_window.h"

// 初始化滑动窗口数组
SlidingWindow *create_sliding_window(int initial_capacity) {
    SlidingWindow *sw = (SlidingWindow *)malloc(sizeof(SlidingWindow));
    sw->windows = (char **)malloc(initial_capacity * sizeof(char *));
    sw->size = 0;
    sw->capacity = initial_capacity;
    return sw;
}

// 添加一个滑动窗口到数组
void add_sliding_window(SlidingWindow *sw, int start, int end) {
    // 如果数组已满，则扩容
    if (sw->size == sw->capacity) {
        sw->capacity *= 2;
        sw->windows = (char **)realloc(sw->windows, sw->capacity * sizeof(char *));
    }
    // 生成窗口的字符串并加入数组
    char *window = (char *)malloc(50 * sizeof(char)); // 假设每个窗口字符串长度不会超过50
    snprintf(window, 50, "%d\t%d", start, end);
    sw->windows[sw->size++] = window;
}

// 释放滑动窗口数组
void free_sliding_window(SlidingWindow *sw) {
    for (int i = 0; i < sw->size; i++) {
        free(sw->windows[i]); // 释放每个窗口字符串
    }
    free(sw->windows); // 释放窗口数组指针
    free(sw);          // 释放结构体
}

// 生成滑动窗口
SlidingWindow *generate_sliding_window(int chr_end, int win_start, int win_end, int step) {
    // 创建滑动窗口数组
    SlidingWindow *sw = create_sliding_window(10);

    // 生成滑动窗口
    while (win_end < chr_end) {
        add_sliding_window(sw, win_start, win_end);
        win_start += step;
        win_end += step;
    }
    // 添加最后一个窗口（可能超出 chr_end，需调整）
    if (win_start < chr_end) {
        add_sliding_window(sw, win_start, chr_end);
    }

    return sw;
}
