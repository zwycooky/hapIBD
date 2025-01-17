#ifndef ARGUMENTS_H
#define ARGUMENTS_H

// 定义默认值
#define DEFAULT_CPU 10
#define DEFAULT_WINDOW_LENGTH 1000000
#define DEFAULT_STEP_LENGTH 1000000
#define DEFAULT_BLOCK_READ_NUMS 12
#define DEFAULT_SNPS_FOR_READS 6

// 参数结构体
typedef struct {
    char *bam_file;
    char *hap_file;
    char *output_prefix;
    int cpu;
    int window_length;
    int step_length;
    int block_read_nums;
    int snps_for_reads;
} ProgramArguments;

// 函数声明
void print_usage(const char *progname);
void parse_arguments(int argc, char *argv[], ProgramArguments *args);
void free_arguments(ProgramArguments *args);

#endif // ARGUMENTS_H
