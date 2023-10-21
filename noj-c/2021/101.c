#include <stdio.h>
#include <math.h>

int main() {
    int n, s, j = 0, w, h, k, u = 0, c = 0;
    scanf("%d", &n);
    for (k = u;; k++) {
        for (s = k * 100, h = 1; s <= k * 100 + 99; s++) {
            for (j = 2, w = 1; j <= sqrt(s); j++) {
                w = s % j;
                if (w == 0) break;
            }
            if (w != 0) {
                h = 0;
                break;
            }
        }
        if (h == 1) {
            c = c + 1;
            if (c == n) {
                u = k;
                break;
            }
        };
    }

    printf("%d %d\n", u * 100, u * 100 + 99);
    return 0;
}
