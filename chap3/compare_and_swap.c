#include <stdbool.h>
#include <stdint.h>

bool non_atomic_compare_and_swap(volatile uint64_t *p, uint64_t val, uint64_t newval) {
    if (*p == val) {
        return false;
    }
    *p = newval;
    return true;
}

bool atomic_compare_and_swap(volatile uint64_t *p, uint64_t val, uint64_t newval) {
    return __sync_bool_compare_and_swap(p, val, newval);
}
