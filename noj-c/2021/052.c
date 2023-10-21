#include <stdio.h>
#include <stdlib.h>

int main() {
    int x, y, k, a, b;
    scanf("%d", &k);
    a = k * (k + 1);
    x = a;
    while (x <= a && x > k) {
        y = x;
        while ((y <= x) && (y > k)) {
            b = k * x + k * y;
            if (x * y == b) printf("1/%d=1/%d+1/%d\n", k, x, y);
            y = y - 1;
        };
        x = x - 1;
    };
    return 0;
}
