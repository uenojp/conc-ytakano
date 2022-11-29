#include <stdint.h>

static volatile uint64_t semaphore_count = 0;
static const uint64_t semaphore_count_limit = 5;

void semaphore_acquire() {
    for (;;) {
        while (semaphore_count >= semaphore_count_limit)
            ;
        __sync_fetch_and_add(&semaphore_count, 1);
        if (semaphore_count <= semaphore_count_limit)
            break;
        __sync_fetch_and_sub(&semaphore_count, 1);
    }
}

void semaphore_release() {
    __sync_fetch_and_sub(&semaphore_count, 1);
}

//
// TEST
//
#ifdef TEST

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void assert_le(uint64_t lhs, uint64_t rhs);

volatile static uint64_t count = 0;

static void* update_count(void* args) {
    for (size_t i = 0; i < 1000; i++) {
        // semaphoreによりクリティカルセクションに入るスレッドを`semaphore_count_limit`以下に制限
        semaphore_acquire();

        /************* CS ****************/
        __sync_fetch_and_add(&count, 1);

        // `semaphore_count_limit`個のスレッドがここまで到達したとき
        // `count`はちょうど`semaphore_count_limit`
        assert_le(count, semaphore_count_limit);

        usleep(1000);
        __sync_fetch_and_sub(&count, 1);
        /************* CS ****************/

        semaphore_release();

        // あるスレッドがCS抜けた直後は`count`が`semaphore_count_limit - 1`以下
        // その抜けたスレッドが`__sync_fetch_and_sub(&count, 1)`したため
    }

    pthread_exit(NULL);
}

int main() {
    const size_t nthreads = 10;
    pthread_t threads[nthreads];

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_create(&threads[i], NULL, update_count, NULL) != 0) {
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

    pthread_exit(NULL);
}

void assert_le(uint64_t lhs, uint64_t rhs) {
    if (lhs > rhs) {
        fprintf(stderr, "expected rhs <= lhs, but got %ld > %ld\n", lhs, rhs);
        exit(1);
    }
}

#endif
