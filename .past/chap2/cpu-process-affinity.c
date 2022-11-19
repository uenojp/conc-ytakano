#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

void* thread_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("i: %d\n", i);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char** argv) {
    pid_t pid;
    cpu_set_t cpu_set;

    pid = getpid();
    printf("%d\n", pid);

    CPU_ZERO(&cpu_set);
    CPU_SET(9, &cpu_set);
    if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) != 0) {
        perror("sched_satafficity");
        return -1;
    }

    for (;;)
        ;

    return 0;
}