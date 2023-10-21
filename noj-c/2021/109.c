#include <stdio.h>
#include <stdlib.h>

int main() {
    int    a, i, n, f1, f2, f3, f4, f5, f6, m1, m2, m3, m4, m5, m6;
    double F[10];
    for (a = 0; a < 10000; a++) {
        F[1] = a;
        for (i = 2; i < 7; i++) {
            n    = i - 1;
            F[i] = (5 * F[n]) / 4 + 1;
        }
        m1 = F[1] * 10;
        m2 = F[2] * 10;
        m3 = F[3] * 10;
        m4 = F[4] * 10;
        m5 = F[5] * 10;
        m6 = F[6] * 10;
        if (m1 % 10 == 0 && m2 % 10 == 0 && m3 % 10 == 0 && m4 % 10 == 0
            && m5 % 10 == 0 && m6 % 10 == 0) {
            f1 = F[1];
            f2 = F[2];
            f3 = F[3];
            f4 = F[4];
            f5 = F[5];
            f6 = F[6];
            if (f1 % 4 == 0 && f2 % 4 == 0 && f3 % 4 == 0 && f4 % 4 == 0
                && f5 % 4 == 0) {
                printf("%d %d", f6, a);
                break;
            }
        } else
            continue;
    }
    return 0;
}
