#include <stdint.h>

#include "spinlock.c"

static volatile uint64_t nreader = 0;
static volatile uint64_t nwriter = 0;

void rwlock_read_acquire() {
    for (;;) {
        while (nwriter)
            ;
        __sync_fetch_and_add(&nreader, 1);
        if (nwriter)
            break;
        __sync_fetch_and_sub(&nreader, 1);
    }
}

void rwlock_read_release() {
    __sync_fetch_and_sub(&nreader, 1);
}

void rwlock_write_acquire() {
    __sync_fetch_and_add(&nwriter, 1);
    while (nwriter || nreader)
        ;
    spinlock_acquire();
}

void rwlock_write_release() {
    spinlock_release();
    __sync_fetch_and_sub(&nwriter, 1);
}

#define TEST
#ifdef TEST

#include <stdio.h>

void reader() {
    static size_t index = 0;
    for (;;) {
        rwlock_read_acquire();

        // read only

        rwlock_read_release();
    }
}

void write() {
    for (;;) {
        rwlock_write_acquire();

        // read/write

        rwlock_write_release();
    }
}
#endif
