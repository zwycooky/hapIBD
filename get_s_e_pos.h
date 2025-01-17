#ifndef GET_S_E_POS_H
#define GET_S_E_POS_H

// 声明函数接口
int get_s_e_pos(int start_pos, const char *cigar, char **cigar_ops, int *end_pos);
void free_cigar_ops(char **cigar_ops, int count);

#endif
