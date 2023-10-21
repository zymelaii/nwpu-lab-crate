#include <stdio.h>

int main() {
    int a, b, c, d, i;
    scanf("%d%d%d", &a, &b, &c);
    d = a % b;
    printf("%d.", a / b);
    for (i = 0; i < c; i++) {
        d = 10 * d;
        printf("%d", d / b);
        d = d % b;
    }
    printf("\n");
    return 0;
}
