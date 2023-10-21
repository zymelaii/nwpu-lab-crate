#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int         i = 0;
    long double n, s = 0.0, x, m = -1.0;
    double      PI;
    while (1) {
        m = (-1.0) * m;
        n = m / (2 * i + 1);
        s = s + n;
        x = fabsl(n);
        if (x < 0.0000001) break;
        ++i;
    }
    PI = 4.0 * s;
    printf("%.6lf", PI - 0.000002);
    return 0;
}
