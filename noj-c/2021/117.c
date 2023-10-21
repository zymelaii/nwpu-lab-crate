#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int x, y, a, b, l, m;
    scanf("%d%d%d%d%d", &x, &y, &a, &b, &l);
    a = abs(a - b);
    x = l - abs(x - y);
    for (m = 0;; m++)
        if ((x + m * l) % a == 0) {
            printf("%d", ((x + m * l) / a));
            break;
        };
    return 0;
}
