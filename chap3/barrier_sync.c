#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

int num = 0;

void barrier(volatile int* cnt, int max) {
    /* atomicにインクリメント */
    __sync_fetch_and_add(cnt, 1);
    while (*cnt < max)
        ;
}

void* do_something(void* arg) {
    /* 10このスレッドがすべてbarrierに到達して初めて、その先が実行される
      実際、waitingより先に10 threas are readyのメッセージは表示されない */
    printf("waiting...\n");
    barrier(&num, NUM_THREADS);
    printf("%d threads are ready\n", NUM_THREADS);
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t threads[NUM_THREADS];
    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, do_something, NULL) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    return 0;
}