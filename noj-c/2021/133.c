#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m, i, j, k;
    scanf("%d", &n);
    j = n * n * n;
    printf("%d*%d*%d=%d=", n, n, n, j);
    if (n % 2 == 0) {
        m = n * n;
        k = n / 2;
        m = m - (2 * k) + 1;
        for (i = 1; i < n; i++) {
            printf("%d+", m);
            m = m + 2;
        }
        printf("%d", m);
    } else {
        m = n * n;
        k = n / 2;
        m = m - (2 * k);
        for (i = 1; i < n; i++) {
            printf("%d+", m);
            m = m + 2;
        }
        printf("%d", m);
    }
    return 0;
}
