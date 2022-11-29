#include <stdbool.h>
#include <stdint.h>

bool non_atomic_test_and_set(volatile bool *p) {
    if (*p) {
        return true;
    } else {
        *p = true;
        return false;
    }
}

void non_atomic_tas_release(volatile bool *p) {
    *p = false;
}

bool atomic_test_and_set(volatile bool *p) {
    return __sync_lock_test_and_set(p, 1);
}

void atomic_tas_release(volatile bool *p) {
    __sync_lock_release(p);
}
