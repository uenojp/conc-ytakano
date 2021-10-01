/* p73 POSIXセマフォ
   セマフォの最大NプロセスまでCSへ入れるという性質を確認する

   そのために、CS内でカウンタを
    - インクリメント
    - スリープ
   (複数作ったプロセスを並行状態にするため、インク、デクリメントだけでは速すぎて並行状態にならない)
    - デクリメント
   し、カウントの値が常にN以下であることを確認する
   仮に、カウントがN以上になると、インクリメントはCS内の操作であるため
   CSにNプロセス以上が入ったことになる */

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const int NUM_THREADS = 10;
const int NUM_LOOP = 10;  // スレッド内のループ数
const int N = 3;          // CSへ入れるプロセスの数

/* CSに入ったスレッド数(セマフォを扱う際に必要な変数ではない)
   ただCSにいくつスレッドが入ったかを確認したい */
int count = 0;

void* th(void* arg) {
    sem_t* sem = sem_open("/mysemaphore", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open in th");
        exit(1);
    }
    for (int i = 0; i < NUM_LOOP; i++) {
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(1);
        }

        /* クリティカルセクション */

        /* カウンタをatomiにインクリメント */
        __sync_fetch_and_add(&count, 1);
        printf("%d\n", count);

        usleep(10000);

        __sync_fetch_and_sub(&count, 1);

        /* ここまで */

        /* セマフォの値を増やす、つまり、CSに入れるスレッドの数を増やす */
        if (sem_post(sem) == -1) {
            perror("sem_post");
            exit(1);
        }
    }
    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(1);
    }
    return NULL;
}

int main(int argc, char** argv) {
    /* permission deniedで弾かれる
       sudoで実行してもだめだった
       ubuntuのコンテナ立ててそこでやったら(root)できた、なぞ */
    sem_t* s = sem_open("/mysemaphore", O_CREAT, 0066, N);
    if (s == SEM_FAILED) {
        perror("sem_open in main");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, th, NULL) != 0) {
            perror("pthread_create");
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
        }
    }

    printf("%d\n", count);
    return 0;
}