#include <stdio.h>

int main() {
    int i, j, k = 0, h, n, m, a[100], t, o = 0, q;
    scanf("%d", &m);
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (a[j] < a[j + 1]) t = a[j], a[j] = a[j + 1], a[j + 1] = t;
    for (i = 0; i < n - o; i++) {
        q = 0;
        for (j = i + 1; j < n - o; j++)
            if (a[i] + a[j] <= m) {
                k++;
                o++;
                q++;
                for (h = j; h < n - o - 1; h++) a[h] = a[h + 1];
                break;
            };
        if (q == 0) k++;
    }
    printf("%d", k);
    return 0;
}
