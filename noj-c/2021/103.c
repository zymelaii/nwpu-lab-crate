#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int    m, n, x = 1, y, i, j, a, b, c;
    double s = 0, g = 0;
    scanf("%d%d", &m, &n);
    for (b = 1; b < n; b++) { x = x * 10; }
    y = 10 * x;
    for (i = x; i < y; i++) {
        if (i % m == 0) continue;
        for (j = 0; j < n; j++) {
            int k = 1;
            for (c = 1; c <= j; c++) { k = k * 10; }
            a = (i / k) % 10;
            if (a == m) goto love;
        }
        continue;
love : {
    s = s + i;
    g++;
}
    }
    printf("%.0lf %.0lf", g, s);
    return 0;
}
