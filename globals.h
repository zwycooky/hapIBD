#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern int active_threads;

typedef struct {
    char **array;
    int size;
} ThreadResult;


#endif // GLOBALS_H
