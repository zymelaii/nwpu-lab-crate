#include <stdio.h>
#include <stdlib.h>

int n;

double getfloor(double m) {
    n = m;
    if (n != m && n < 0) n--;
    return n;
}

int main() {
    int    a;
    double x;
    scanf("%lf", &x);
    a = getfloor(x);
    printf("%d", a);
    return 0;
}
