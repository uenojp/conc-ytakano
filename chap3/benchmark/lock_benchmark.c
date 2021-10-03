#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "barrier.c"
// #include "empty.c"
#include "mutex.c"
// #include "rwlock.c"
// #include "rwlock_wr.c"

/* バリア同期用変数
   nready: 計測準備ができたスレッドの数
   ndone : 計測終わったスレッドの数
    ndoneがNUM_THREADSと等しくなると、結果を出力*/
volatile int nready = 0;
volatile int ndone = 0;

/* 計測の終了を知らせるフラグ */
volatile bool done = false;

uint64_t count[NUM_THREADS];

void *worker(void *arg) {
    if (barrier(&nready, NUM_THREADS) != 0) {
        perror("barrier");
        exit(1);
    }

    uint64_t id = (uint64_t)arg;
    /* ループした回数 */
    uint64_t n = 0;

    while (done == false) {
        do_lock();
        n++;
    }
    count[id] = n;

    if (barrier(&ndone, NUM_THREADS)) {
        perror("barrier");
        exit(1);
    }

    return NULL;
}

void *timer(void *arg) {
    if (barrier(&nready, NUM_THREADS) != 0) {
        perror("barrier");
        exit(1);
    }

    sleep(10);
    done = true;

    if (barrier(&ndone, NUM_THREADS) != 0) {
        perror("barrier");
        exit(1);
    }

    for (uint64_t i = 0; i < NUM_THREADS - 1; i++) {
        printf("%lu\n", count[i]);
    }

    return NULL;
}

int main(int argc, char **argv) {
    for (uint64_t i = 0; i < NUM_THREADS - 1 /* -1 for timer */; i++) {
        pthread_t th;
        pthread_create(&th, NULL, worker, (void *)i);
        pthread_detach(th);
    }

    pthread_t th;
    pthread_create(&th, NULL, timer, NULL);
    pthread_join(th, NULL);

    return 0;
}