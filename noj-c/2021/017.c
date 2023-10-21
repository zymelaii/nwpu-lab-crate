#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pi 3.1415926

int f(float x, float y) {
    int i;
    for (i = 0;; i++)
        if (50 * i > sqrt(x * x + y * y) * sqrt(x * x + y * y) * pi / 2) break;
    return i;
}

int main() {
    int   n, i, a[100];
    float x, y;
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%f%f", &x, &y);
        a[i] = f(x, y);
    }
    for (i = 0; i < n; i++) printf("%d %d\n", i + 1, a[i]);
    return 0;
}
