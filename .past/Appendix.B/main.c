#include <stdio.h>

extern unsigned int fact(unsigned int);
extern unsigned int sum(unsigned int);

int main() {
    unsigned int n = 10;
    printf("fact(%d): %d\n", n, fact(n));
    printf("sum(%d): %d\n", n, sum(n));
    return 0;
}
