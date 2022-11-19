void wait_while_o(volatile int *p) {
    while (*p == 0) {}
}