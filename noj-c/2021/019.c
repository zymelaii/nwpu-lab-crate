#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int a[1000], b[1000], n, i, j, s, sum, t, m, mid, c[100], k = 0;
    while (1) {
        scanf("%d", &n);
        if (n == 0) break;
        {
            s = 0;
            for (i = 1; i <= n; i++) {
                scanf("%d", &a[i]);
                s = s + a[i];
            }
            m    = s / n;
            b[1] = a[1] - m;
            b[0] = 0;
            for (i = 2; i < n; ++i) b[i] = b[i - 1] + a[i] - m;
            for (i = 0; i < n; i++)
                for (j = 0; j < n - 1 - i; j++)
                    if (b[j] > b[j + 1])
                        t = b[j], b[j] = b[j + 1], b[j + 1] = t;
            mid = b[n / 2];
            sum = 0;
            for (i = 0; i <= n - 1; ++i) sum = sum + abs(mid - b[i]);
            c[k] = sum;
            k++;
        }
    }
    for (i = 0; i < k; i++) printf("%d\n", c[i]);
    return 0;
}
