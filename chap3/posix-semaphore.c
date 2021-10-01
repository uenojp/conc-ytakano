/* p73 POSIXセマフォ
   セマフォの最大NプロセスまでCSへ入れるという性質を確認する

   そのために、CS内でカウンタを
    - インクリメント
    - スリープ (複数作ったプロセスを並行状態にするため、インク、デクリメントだけでは速すぎて並行状態にならない)
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
const int N;              // CSへ入れるプロセスの数

int count = 0;

void* th(void* arg) {
    for (int i = 0; i < NUM_LOOP; i++) {
        /* クリティカルセクション*/
        count += 1;
        usleep(10000);
        count += 1;
        /* ここまで */
    }
    return NULL;
}

int main(int argc, char** argv) {
    sem_t* s = sem_open("/mysemaphore", O_CREAT, 0066, N);
    if (s == SEM_FAILED) {
        perror("sem_open");
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