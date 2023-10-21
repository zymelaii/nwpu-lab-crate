#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int i, j;
    for (i = 1; i <= 1000; i++) {
        int F[1000], m = 0, n, a, X = 0;
        for (j = 1; j < i; j++) {
            if (i % j == 0) {
                m    = m + 1;
                F[m] = j;
            }
        }
        for (n = 1; n <= m; n++) X = X + F[n];
        if (X == i) {
            printf("%d=", i);
            for (a = 1; a < m; a++) printf("%d+", F[a]);
            printf("%d\n", F[m]);
        }
    }
    return 0;
}
