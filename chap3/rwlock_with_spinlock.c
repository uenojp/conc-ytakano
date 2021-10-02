#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void spinlock_acquire(volatile bool *lock) {
    for (;;) {
        while (*lock)
            ;
        if (!__sync_lock_test_and_set(lock, 1))
            break;
    }
}

void spinlock_release(volatile bool *lock) {
    __sync_lock_release(lock);
}

void rwlock_read_acquire(int *rcnt, int *wcnt) {
    for (;;) {
        while (*wcnt)
            ;
        __sync_fetch_and_add(rcnt, 1);
        if (*wcnt == 0)
            break;
        __sync_fetch_and_sub(rcnt, 1);
    }
}

void rwlock_read_release(int *rcnt) {
    __sync_fetch_and_sub(rcnt, 1);
}

void rwlock_write_acquire(bool *lock, int *rcnt, int *wcnt) {
    /* readerがいるかどうかの状況にかかわらず、カウントを増やす
       つまり、readerよりwirterを優先する
       whileが先だと、reader優先になる(readerがいなくなるまで入れない) */
    __sync_fetch_and_add(wcnt, 1);
    while (*rcnt)
        ;
    spinlock_acquire(lock);
}

void rwlock_write_release(bool *lock, int *wcnt) {
    spinlock_release(lock);
    __sync_fetch_and_sub(wcnt, 1);
}

int nr = 0;
int nw = 0;
bool lock = false;

int count = 0;

/* rwlockの性質をすべて表しているとは言えないが
   readerがロックを一つでも獲得していると、writerは入れない
   ことが、readerでsleepすることでわかる */
void *reader(void *arg) {
    rwlock_read_acquire(&nr, &nw);

    /* CS readonly */
    usleep(100000);
    for (size_t i = 0; i < 3; i++) {
        printf("read count: %d\n", count);
    }

    rwlock_read_release(&nr);
}

void *writer(void *arg) {
    rwlock_write_acquire(&lock, &nr, &nw);

    /* CS read or write */
    for (size_t i = 0; i < 3; i++) {
        count += 1;
        printf("write count: %d\n", count);
    }

    rwlock_write_release(&lock, &nw);
}

int main(int argc, char **argv) {
    pthread_t rth, wth;
    pthread_create(&rth, NULL, reader, NULL);
    pthread_create(&wth, NULL, writer, NULL);

    pthread_join(rth, NULL);
    pthread_join(wth, NULL);

    return 0;
}