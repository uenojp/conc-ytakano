#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

int count = 0;

void* reader(void* arg) {
    /* readerのロック獲得が遅れて、先にwriteにロック取られたら
       writeが終わるまでreaderは一切入れない */
    if (pthread_rwlock_rdlock(&rwlock) != 0) {
        perror("pthread_rwlock_rdlock");
        exit(1);
    }

    /* CS readonly */
    printf("read count: %d\n", count);

    if (pthread_rwlock_unlock(&rwlock) != 0) {
        perror("pthread_rwlock_unlock");
        exit(1);
    }
    return NULL;
}

void* writer(void* arg) {
    if (pthread_rwlock_wrlock(&rwlock) != 0) {
        perror("pthread_rwlock_wrlock");
        exit(1);
    }

    /* CSs read or write */
    count += 1;
    printf("write count: %d\n", count);

    if (pthread_rwlock_unlock(&rwlock) != 0) {
        perror("pthread_rwlock_unlock");
        exit(1);
    }
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t rd[3], wr[3];
    for (size_t i = 0; i < 3; i++) {
        pthread_create(&rd[i], NULL, reader, NULL);
        pthread_create(&wr[i], NULL, writer, NULL);
    }

    for (size_t i = 0; i < 3; i++) {
        pthread_join(rd[i], NULL);
        pthread_join(wr[i], NULL);
    }

    if (pthread_rwlock_destroy(&rwlock) != 0) {
        perror("pthread_rwlock_destroy");
        return 1;
    }
    return 0;
}