#include "utils.h"

int generate_random_number(int max) {
    return rand() % max + 1;
}

bool directory_exists(const char *path) {
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

bool create_directory(const char *path) {
    if (mkdir(path, 0755) == 0 || errno == EEXIST) {
        return true;
    }
    perror("mkdir failed");
    return false;
}

char *get_absolute_path(const char *path) {
    static char abs_path[PATH_MAX];
    if (realpath(path, abs_path)) {
        return abs_path;
    } else {
        perror("realpath failed");
        return NULL;
    }
}

