#include "test_and_set.c"

static volatile bool lock = false;

//
// atomic
//
void spinlock_acquire() {
    for (;;) {
        while (lock)
            ;
        if (!atomic_test_and_set(&lock))
            break;
    }
}

void spinlock_release() {
    atomic_tas_release(&lock);
}

//
// non atomic
//
void bad_spinlock_acquire() {
    for (;;) {
        while (lock)
            ;
        if (!non_atomic_test_and_set(&lock))
            break;
    }
}

void bad_spinlock_release() {
    non_atomic_tas_release(&lock);
}

//
// TEST
//
#ifdef TEST

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void assert_eq(uint64_t expected, uint64_t actual);

volatile static uint64_t count = 0;

static void* increment(void* args) {
    for (size_t i = 0; i < 10000; i++) {
        spinlock_acquire();
        // bad_spinlock_acquire();
        count++;
        spinlock_release();
        // bad_spinlock_release();
    }

    pthread_exit(NULL);
}

int main() {
    const size_t nthreads = 10;
    pthread_t threads[nthreads];

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, increment, NULL) != 0) {
            perror("pthread_create");
            return -1;
        }
    }

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthraed_join");
            return -1;
        }
    }

    assert_eq(10000 * 10, count);

    pthread_exit(NULL);
}

void assert_eq(uint64_t expected, uint64_t actual) {
    if (expected != actual) {
        fprintf(stderr, "expected %ld, but got %ld\n", expected, actual);
        exit(1);
    }
}

#endif
