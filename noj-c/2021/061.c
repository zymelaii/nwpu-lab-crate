#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m, b, c, d, e;
    while (n > 1) {
        if ((n - 1) % 5 == 0) {
            b = (n - 1) / 5 * 4;
            if ((b - 1) % 5 == 0) {
                c = (b - 1) / 5 * 4;
                if ((c - 1) % 5 == 0) {
                    d = (c - 1) / 5 * 4;
                    if ((d - 1) % 5 == 0) {
                        e = (d - 1) / 5 * 4;
                        if ((e - 1) % 5 == 0) break;
                    }
                }
            }
        };
        n = n + 1;
    }
    m = (e - 1) / 5 * 4;
    printf("%d %d", n, m);
    return 0;
}
