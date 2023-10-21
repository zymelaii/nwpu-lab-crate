#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double a, b, c, d;
    scanf("%lf", &a);
    b = a;
    c = 1;
    while ((fabs(b - c)) > 0.000001) {
        d = (b + (a / b)) / 2;
        c = b;
        b = d;
    };
    printf("%.5lf", b);
    return 0;
}
