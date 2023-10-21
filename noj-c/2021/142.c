#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, c;
    scanf("%d%d", &a, &b);
    c = 1;
    while (b > 0) {
        c = c * a;
        c = c % 1000;
        b = b - 1;
    };
    printf("%d", c);
    return 0;
}
