#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double a, b, c, d;
    char   x1, x2, x3, x4, x5, x6;
    scanf("%lf%lf%lf%lf", &a, &b, &c, &d);
    if (b > 0)
        x1 = '+';
    else
        x1 = '-';
    if (d > 0)
        x2 = '+';
    else
        x2 = '-';
    if (b + d > 0)
        x3 = '+';
    else
        x3 = '-';
    if (b - d > 0)
        x4 = '+';
    else
        x4 = '-';
    if (a * d + b * c > 0)
        x5 = '+';
    else
        x5 = '-';
    if ((b * c - a * d) / (c * c + d * d) > 0)
        x6 = '+';
    else
        x6 = '-';
    printf(
        "(%.2lf%c%.2lfi)+(%.2lf%c%.2lfi)=(%.2lf%c%.2lfi)\n",
        a,
        x1,
        fabs(b),
        c,
        x2,
        fabs(d),
        a + c,
        x3,
        fabs(b + d));
    printf(
        "(%.2lf%c%.2lfi)-(%.2lf%c%.2lfi)=(%.2lf%c%.2lfi)\n",
        a,
        x1,
        fabs(b),
        c,
        x2,
        fabs(d),
        a - c,
        x4,
        fabs(b - d));
    printf(
        "(%.2lf%c%.2lfi)*(%.2lf%c%.2lfi)=(%.2lf%c%.2lfi)\n",
        a,
        x1,
        fabs(b),
        c,
        x2,
        fabs(d),
        a * c - b * d,
        x5,
        fabs(a * d + b * c));
    printf(
        "(%.2lf%c%.2lfi)/(%.2lf%c%.2lfi)=(%.2lf%c%.2lfi)",
        a,
        x1,
        fabs(b),
        c,
        x2,
        fabs(d),
        (a * c + b * d) / (c * c + d * d),
        x6,
        fabs((b * c - a * d) / (c * c + d * d)));
    return 0;
}
