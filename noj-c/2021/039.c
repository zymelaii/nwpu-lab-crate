#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double a, b, c;
    scanf("%lf", &a);
    if (a > 2) {
        if (a > 15) {
            c = ceil(a - 15);
            b = 26.5 + 2.1 * c;
            printf("%.6lf", b);
        } else {
            c = ceil(a - 2);
            b = 7 + 1.5 * c;
            printf("%.6lf", b);
        }
    } else
        printf("7.000000");
    return 0;
}
