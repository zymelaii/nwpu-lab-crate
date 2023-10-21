#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int f(int n) {
    int s = 1;
    while (1) {
        if (n == 1)
            return s;
        else if (n % 2 == 0)
            n = n / 2, s++;
        else
            n = 3 * n + 1, s++;
    }
}

int main() {
    int n, m, i, j = 0, t;
    scanf("%d%d", &m, &n);
    printf("%d %d", m, n);
    if (m > n) t = m, m = n, n = t;
    for (i = m; i <= n; i++)
        if (f(i) > j) j = f(i);
    printf(" %d", j);
    return 0;
}
