#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

// 定义滑动窗口的结构体
typedef struct {
    char **windows;  // 存储滑动窗口的字符串数组
    int size;        // 当前窗口数量
    int capacity;    // 动态数组容量
} SlidingWindow;

// 创建滑动窗口数组
SlidingWindow *create_sliding_window(int initial_capacity);

// 添加一个滑动窗口到数组
void add_sliding_window(SlidingWindow *sw, int start, int end);

// 释放滑动窗口数组
void free_sliding_window(SlidingWindow *sw);

// 生成滑动窗口
SlidingWindow *generate_sliding_window(int chr_end, int win_start, int win_end, int step);

#endif // SLIDING_WINDOW_H
