#include <stdio.h>
#include <stdlib.h>

int main() {
    double n = 3.0, b = -1;
    double a = 1.0;
    while ((1 / n) >= 1e-6) {
        a = a + (b / n);
        b = b * (-1);
        n = n + 2;
    }
    a = a * 4;
    printf("%.6lf", a);
    return 0;
}
