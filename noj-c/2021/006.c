#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int g(int n) {
    int i;
    if (n == 1) return 0;
    if (n == 2) return 1;
    if (n == 3) return 1;
    for (i = 2; i <= sqrt(n); i++)
        if (n % i == 0) return 0;
    return 1;
}

int f(int a, int b, int c) {
    int i = 0, s = a - b;
    if (c == 1 && g(a) == 1) return a;
    if (b == 0 && g(a) != 1) return -1;
    while (1) {
        s = s + b;
        if (g(s)) i++;
        if (i >= c) break;
    }
    return s;
}

int main() {
    int a, b, c, d[100], i = 0, n;
    while (1) {
        scanf("%d%d%d", &a, &b, &c);
        if (a == 0 && b == 0 && c == 0) break;
        d[i] = f(a, b, c);
        i++;
    }
    n = i;
    for (i = 0; i < n; i++) printf("%d\n", d[i]);
    return 0;
}
