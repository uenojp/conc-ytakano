#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile uint64_t count = 0;

void barrier(uint64_t max) {
    if (pthread_mutex_lock(&mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }

    count++;

    if (count >= max) {
        if (pthread_cond_broadcast(&cond) != 0) {
            perror("pthread_cond_broadcast");
            exit(1);
        }
    } else {
        do {
            if (pthread_cond_wait(&cond, &mutex) != 0) {
                perror("pthread_cond_wait");
                exit(1);
            }
        } while (count < max);  // prevent spurious wakeup
    }

    if (pthread_mutex_unlock(&mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

//
// TEST
//
#ifdef TEST

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

void* worker(void* args) {
    printf("pre:  %ld\n", count);

    barrier(10);

    printf("post: %ld\n", count);

    pthread_exit(NULL);
}

int main(void) {
    const size_t nthreads = 10;
    pthread_t threads[nthreads];

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, worker, NULL) != 0) {
            perror("pthread_create");
            return -1;
        }
    }

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthraed_join");
            return -1;
        }
    }

    pthread_exit(NULL);
}

#endif
