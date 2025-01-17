#ifndef PHASING_READS_H
#define PHASING_READS_H

#define MAX_READ_LENGTH 160
#define MAX_CIGAR_LENGTH 400

// Function prototypes
//char *phasing_reads(char **tmp, char **tmp_hap, int reads_count, int hap_count, int snp_nums_for_reads);
typedef struct {
    int phasedhap;
    int max_block_id;
    double accuracy;
} PhasingRes;

void *phasing_reads(char **tmp, char **tmp_hap, int *tmp_hap_pos, int reads_count, int hap_count, int snp_nums_for_reads, PhasingRes *result);

#endif
