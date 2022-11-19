/* 条件変数は待機を扱う、つまり、プロセス間の順序を規定する */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
/* readyの必要性がまだわかっていない */
bool ready = false;

char buf[256];

void* produce(void* arg) {
    printf("produce ");
    fgets(buf, sizeof(buf), stdin);

    pthread_mutex_lock(&mut);
    ready = true;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);

    return NULL;
}

void* consume(void* arg) {
    pthread_mutex_lock(&mut);
    while (!ready) {
        pthread_cond_wait(&cond, &mut);
    }
    pthread_mutex_unlock(&mut);

    printf("consume %s", buf);

    return NULL;
}

int main(int argc, char** argv) {
    pthread_t prod_th, cons_th;
    if (pthread_create(&prod_th, NULL, produce, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }
    if (pthread_create(&cons_th, NULL, consume, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    pthread_join(prod_th, NULL);
    pthread_join(cons_th, NULL);

    return 0;
}
