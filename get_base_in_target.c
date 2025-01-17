#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "get_base_in_target.h"

int parse_cigar_number(const char *cigar_op) {
    int num = 0;
    while (isdigit(*cigar_op)) {
        num = num * 10 + (*cigar_op - '0');
        cigar_op++;
    }
    return num;
}

int get_base_in_target(int pos, char **cigar_ops, int cigar_count) {
    int base_num = 0;   // Position on reads
    int contig_num = 0; // Position on contig

    for (int i = 0; i < cigar_count; i++) {
        char *cigar_op = cigar_ops[i];
        int num = parse_cigar_number(cigar_op);
        char op = cigar_op[strlen(cigar_op) - 1];
	
        if (op == 'S') {
            base_num += num;
        } else if (op == 'H') {
            continue;
        } else if (op == 'I') {
            base_num += num;
        } else if (op == 'D' || op == 'N') {
            if (pos > contig_num && pos < contig_num + num) {
                // Position is in a deletion region: return error
                return -1; // Use -1 to represent "no" in Perl
            }
            contig_num += num;
        } else if (op == 'M') {
            if (pos > contig_num && pos < contig_num + num) {
                int add_num = pos - contig_num;
                int reads_pos = add_num + base_num;
                return reads_pos;
            } else {
                base_num += num;
                contig_num += num;
            }
        } else {
            fprintf(stderr, "Cannot recognize CIGAR operation: %s\n", cigar_op);
            exit(EXIT_FAILURE);
        }
    }

    return -1; // If position not found, return error
}
