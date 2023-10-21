#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int    n, m, i;
    double sum = 0;
    scanf("%d %d", &n, &m);
    for (i = n; i <= m; i++) { sum = sum + 1 / pow(i, 2.0); }
    printf("%.5lf", sum);

    return 0;
}
