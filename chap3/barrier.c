#include <stdint.h>

volatile uint64_t count = 0;

void barrier(uint64_t max) {
    __sync_fetch_and_add(&count, 1);
    while (count < max)
        ;
}

//
// TEST
//
#ifdef TEST

#include <pthread.h>
#include <stdio.h>
#include <errno.h>

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
