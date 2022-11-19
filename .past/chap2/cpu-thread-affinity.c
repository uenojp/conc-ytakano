#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

void* thread_func(void* arg) {
    for (;;)
        ;
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t thread;
    cpu_set_t cpu_set;
    pthread_attr_t attr;

    if (pthread_attr_init(&attr) != 0) {
        perror("pthread_attr_init");
        return -1;
    }

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        return -1;
    }

    CPU_ZERO(&cpu_set);
    CPU_SET(9, &cpu_set);
    if (pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpu_set) != 0) {
        perror("pthread_attr_setaffinity_np");
        return -1;
    }

    if (pthread_create(&thread, &attr, thread_func, NULL) != 0) {
        perror("pthread_create");
        return -1;
    }

    if (pthread_attr_destroy(&attr) != 0) {
        perror("pthread_attr_destroy");
        return -1;
    }

    // メインスレッドでもループ
    for (;;)
        ;

    // detachしているから必要ない、スレッドが終了すればメモリは自動開放
    // pthread_join(thread, NULL);

    return 0;
}