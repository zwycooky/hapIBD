#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

// Function to generate a random number within a range
int generate_random_number(int max);

// Function to check if a directory exists
bool directory_exists(const char *path);

// Function to create a directory
bool create_directory(const char *path);

// Function to get the absolute path of a directory
char *get_absolute_path(const char *path);

#endif // UTILS_H

