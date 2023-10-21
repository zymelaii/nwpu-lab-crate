#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double a, b, c, sum, avg;
    scanf("%lf%lf%lf", &a, &b, &c);
    sum = a + b + c;
    avg = sum / 3.0;
    printf("%.6lf\n%.6lf\n", sum, avg);
    return 0;
}
