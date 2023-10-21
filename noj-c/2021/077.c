#include <stdio.h>
#include <stdlib.h>

void paixu(int a[], int n) {
    int i, k, t;
    for (i = 1; i < n; i++) {
        t = a[i];
        k = i - 1;
        while (t < a[k]) {
            a[k + 1] = a[k];
            k--;
            if (k == -1) break;
        }
        a[k + 1] = t;
    }
}

int main() {
    int a[100], b[100], n, m, i, j, s, h, q;
    scanf("%d%d", &n, &m);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < m; i++) scanf("%d", &b[i]);
    paixu(a, n);
    paixu(b, m);
    s = 0;
    for (i = 0, h = -1; i < n; i++) {
        for (j = h + 1; j < m; j++) {
            if (b[j] >= a[i]) {
                s = s + b[j];
                h = j;
                q = 2;
                break;
            }
        }
        if (q != 2) q = 1;
    }
    if (q == 1)
        printf("NULL");
    else
        printf("%d", s);
    return 0;
}
