#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int fceil(double x) {
    if (x > 0)
        x = ((int)x) + 1;
    else
        x = (int)x;
    return x;
}

int main() {
    double x;
    int    n;
    scanf("%lf", &x);
    n = fceil(x);
    printf("%d", n);
    return 0;
}
