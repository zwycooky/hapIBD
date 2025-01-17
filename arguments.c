#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "arguments.h"

// 打印用法说明
void print_usage(const char *progname) {
    fprintf(stderr,
        "Usage:\n  %s -OPTIONS VALUES\n\n"
        "options:\n"
        "--input options\n"
        "    -i FILE         bam file\n"
        "    -p FILE         hap file\n\n"
        "--output options\n"
        "    -o STR          outputprefix\n\n"
        "--criteria options\n"
        "    -w INT          window length (bp) [default: 1000000]\n"
        "    -s INT          window step (bp)   [default: 1000000]\n"
        "    -N INT          minimum read numbers of hap block [default: 12]\n"
        "    -n INT          minimum phased numbers of phased snps for reads [default: 6]\n\n"
        "--performance options\n"
        "    -a INT          cpus cores used for the analysis [default: 10]\n\n",
        progname);
}

// 解析命令行参数
void parse_arguments(int argc, char *argv[], ProgramArguments *args) {
    // 初始化默认值
    args->bam_file = NULL;
    args->hap_file = NULL;
    args->output_prefix = NULL;
    args->cpu = DEFAULT_CPU;
    args->window_length = DEFAULT_WINDOW_LENGTH;
    args->step_length = DEFAULT_STEP_LENGTH;
    args->block_read_nums = DEFAULT_BLOCK_READ_NUMS;
    args->snps_for_reads = DEFAULT_SNPS_FOR_READS;

    int opt;
    while ((opt = getopt(argc, argv, "i:p:o:w:s:N:n:a:")) != -1) {
        switch (opt) {
            case 'i':
                args->bam_file = strdup(optarg);
                break;
            case 'p':
                args->hap_file = strdup(optarg);
                break;
            case 'o':
                args->output_prefix = strdup(optarg);
                break;
            case 'w':
                args->window_length = atoi(optarg);
                break;
            case 's':
                args->step_length = atoi(optarg);
                break;
            case 'N':
                args->block_read_nums = atoi(optarg);
                break;
            case 'n':
                args->snps_for_reads = atoi(optarg);
                break;
            case 'a':
                args->cpu = atoi(optarg);
                break;
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // 检查必需参数
    if (!args->bam_file || !args->hap_file || !args->output_prefix) {
        fprintf(stderr, "Error: Missing required arguments.\n\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

// 释放分配的内存
void free_arguments(ProgramArguments *args) {
    if (args->bam_file) free(args->bam_file);
    if (args->hap_file) free(args->hap_file);
    if (args->output_prefix) free(args->output_prefix);
}
