#include <stdio.h>

double f(double x) {
    return 1.0 / (1.0 + x * x);
}

double jf(double a, double b) {
    double h, s = 0;
    ;
    int i;
    h = (b - a) / 5000000;
    for (i = 1; i < 5000000; i++) {
        s += (f(a) + f(a + h)) * 0.5 * h;
        a += h;
    }
    return s;
}

int main() {
    double a, b, s;
    scanf("%lf%lf", &a, &b);
    s = jf(a, b);
    printf("%lf", s);
    return 0;
}
