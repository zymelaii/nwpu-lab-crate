#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, c, d, e, m, n;
    scanf("%d", &m);
    a = m % 10;
    b = ((m - a) % 100) / 10;
    c = ((m - a - 10 * b) % 1000) / 100;
    d = ((m - a - 10 * b - 100 * c) % 10000) / 1000;
    e = ((m - a - 10 * b - 100 * c - 1000 * d) % 100000) / 10000;
    if (e == 0) {
        if (d == 0) {
            if (c == 0) {
                if (b == 0) {
                    if (a == 0)
                        n = 0;
                    else
                        n = a;
                } else
                    n = 10 * a + b;
            } else
                n = 100 * a + 10 * b + c;
        } else
            n = 1000 * a + 100 * b + 10 * c + d;
    } else {
        n = 10000 * a + 1000 * b + 100 * c + 10 * d + e;
    }
    if (m == n) {
        printf("yes");
    } else {
        printf("no");
    }
    return 0;
}
