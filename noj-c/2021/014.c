#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int    a[100], b[100], n, i, j;
    double s, p, l, t;
    for (i = 0;; i++) {
        scanf("%d%d", &a[i], &b[i]);
        if (a[i] == 0 && b[i] == 0) break;
    }
    n = i;
    for (i = 0; i < n; i++) {
        p = 10000;
        if (b[i] % a[i] == 0) {
            printf("0.0000\n");
            continue;
        };
        t = 10000 / ((double)a[i]);
        for (j = 1; j < a[i] + b[i]; j++) {
            l = 10000 / ((double)(a[i] + b[i]));
            l = t - j * l;
            l = fabs(l);
            if (l < p) p = l;
        }
        s = (a[i] - 1) * p;
        printf("%.4lf\n", s);
    }
    return 0;
}
