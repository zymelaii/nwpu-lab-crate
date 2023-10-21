#include <stdio.h>
#include <stdlib.h>

int main() {
    int    n;
    double x, s = 0, m = 0, i;
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%lf", &x);
        s = s + x;
        m = m + x * x;
    }
    m = m - (2 * s * s) / n * 1.0 + s * (s / n * 1.0);
    printf("%.6lf", m);
    return 0;
}
