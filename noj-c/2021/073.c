#include <stdio.h>
#include <stdlib.h>

int main() {
    double a, b;
    scanf("%lf", &a);
    if (a > 10) {
        if (a > 20) {
            if (a > 40) {
                if (a > 60) {
                    if (a > 100) {
                        b = 3.95 + (a - 100) * 0.01;
                    } else
                        b = 3.35 + (a - 60) * 0.015;
                } else
                    b = 2.75 + (a - 40) * 0.03;
            } else
                b = 1.75 + (a - 20) * 0.05;
        } else
            b = 1 + (a - 10) * 0.075;
    } else
        b = a * 0.1;
    printf("%.6lf", b);
    return 0;
}
