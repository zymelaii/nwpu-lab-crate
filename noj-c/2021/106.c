#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m, b, c, d;
    scanf("%d", &n);
    m = 3;
    b = 0;
    while (m <= n) {
        if ((n - m) == 1) b = b + 2;
        if ((n - m) == 2) b = b + 1;
        if (n == m) b = b + 1;
        c = n - m - 1;
        d = 0;
        if (c > 0) {
            d = 1;
            while (c > 0) {
                d = d * 2;
                c = c - 1;
            };
        };
        b = b + 2 * d;
        c = n - m - 2;
        d = 0;
        if (c > 0) {
            d = 1;
            while (c > 0) {
                d = d * 2;
                c = c - 1;
            };
        };
        b = b + (n - m - 1) * d;
        m = m + 1;
    };
    printf("%d", b);
    return 0;
}
