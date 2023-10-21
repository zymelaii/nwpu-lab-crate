#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int f(int n) {
    int a[100], b[100], i, s;
    for (i = 0; i < n; i++) scanf("%d%d", &a[i], &b[i]);
    s = a[0] * b[0];
    for (i = 1; i < n; i++) s = s + a[i] * (b[i] - b[i - 1]);
    return s;
}

int main() {
    int n, c[100], i = 0;
    while (1) {
        scanf("%d", &n);
        if (n == -1) break;
        c[i] = f(n);
        i++;
    }
    n = i;
    for (i = 0; i < n; i++) printf("%d\n", c[i]);
    return 0;
}
