#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double a;
    int    m, d, c, n;
    scanf("%lf", &a);
    a = fabs(a);
    n = (int)a;
    m = 0;
    d = 1;
    if (n < 1) {
        printf("0");
        return 0;
    };
    do {
        d = d * 10;
        c = n - d;
        m = m + 1;
    } while (c > 0);
    printf("%d", m);
    return 0;
}
