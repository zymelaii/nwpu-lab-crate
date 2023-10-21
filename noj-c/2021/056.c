#include <stdio.h>
#include <stdlib.h>

inline int xchg(unsigned char n) {
    return 16 * (n - ((n >> 4) * 16)) + (n >> 4);
}

int main() {
    int x;
    scanf("%d", &x);
    printf("%d", xchg(x));
    return 0;
}
