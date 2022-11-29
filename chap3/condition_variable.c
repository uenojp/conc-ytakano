#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char buf[256];

bool ready = false;

void* producer(void* args) {
    printf("producer: ");
    fgets(buf, sizeof(buf), stdin);
    ready = true;

    pthread_exit(NULL);
}

void* consumer(void* args) {
    while (!ready)
        ;
    printf("consumer: %s", buf);

    pthread_exit(NULL);
}

int main(void) {
    pthread_t pr;
    pthread_t co;

    if (pthread_create(&pr, NULL, producer, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&co, NULL, consumer, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    pthread_exit(NULL);
}
