#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int    n, x, y;
    double sum = 0, i;
    scanf("%d", &n);
    for (i = 1; i <= n; i++) {
        x   = pow((-1), i);
        y   = pow(2, i);
        sum = sum - (x * y * 1.0) / ((x + y) * (2 * y - x));
    }
    printf("%.6lf", sum);
    return 0;
}
