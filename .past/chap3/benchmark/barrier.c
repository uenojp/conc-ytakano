#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/* barrier waits until max threads have reached barrier */
int barrier(volatile int* cnt, int max) {
    if (pthread_mutex_lock(&mut) != 0)
        return 1;

    (*cnt)++;

    if (*cnt >= max) {
        if (pthread_cond_broadcast(&cond) != 0)
            return 1;
    } else {
        do {
            if (pthread_cond_wait(&cond, &mut) != 0)
                return 1;
        } while (!(*cnt >= max));
    }

    if (pthread_mutex_unlock(&mut) != 0)
        return 1;

    return 0;
}