#include <stdio.h>
#include <stdlib.h>

int main() {
    double m, n, a, b, c, x;
    scanf("%lf%lf", &m, &n);
    while (c != 0) {
        x = (m + n) / 2;
        a = 2 * m * m * m - 4 * m * m + 3 * m - 6;
        b = 2 * n * n * n - 4 * n * n + 3 * n - 6;
        c = 2 * x * x * x - 4 * x * x + 3 * x - 6;
        if (a * c > 0)
            m = x;
        else
            n = x;
    }
    printf("%.2lf\n", x);
    return 0;
}
