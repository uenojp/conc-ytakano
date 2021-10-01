#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 3

/* barrierに到達したスレッド数 */
int num = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void barrier(volatile int *cnt, unsigned int max) {
    if (pthread_mutex_lock(&mut) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }

    (*cnt)++;

    /* 最後にここへ来たスレッドのみ下のif文を満たす */
    if (*cnt >= max) {
        if (pthread_cond_broadcast(&cond) != 0) {
            perror("pthread_cond_broadcast");
            exit(1);
        }
    } else {
        do {
            if (pthread_cond_wait(&cond, &mut) != 0) {
                perror("pthread_cond_wait");
                exit(1);
            }
        } while (*cnt < max);
        /* pthread_cond_wait抜けたあと、もう一度条件を満たしているか確認
           満たしていなければもう一度ループへ戻る
           条件とは、pthread_cond_waitを抜ける条件
           = broadcastする条件 = *cnt >= max
           つまり、!(*cnt >= max) = *cnt < maxならループ再開*/
    }

    if (pthread_mutex_unlock(&mut) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

void *th(void *arg) {
    printf("waiting...\n");
    barrier(&num, NUM_THREADS);
    printf("ready\n");
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t threads[NUM_THREADS];
    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, th, NULL) != 0) {
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