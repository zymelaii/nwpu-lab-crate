#include <stdio.h>
#include <stdlib.h>

int main() {
    double a, b, c, d, e, f;
    scanf("%lf,%lf", &a, &b);
    c = (a - 2) * (a - 2) + (b - 2) * (b - 2);
    d = (a + 2) * (a + 2) + (b - 2) * (b - 2);
    e = (a - 2) * (a - 2) + (b + 2) * (b + 2);
    f = (a + 2) * (a + 2) + (b + 2) * (b + 2);
    if (c <= 1 || d <= 1 || e <= 1 || f <= 1)
        printf("10");
    else
        printf("0");
    return 0;
}
