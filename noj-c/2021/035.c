#include <stdio.h>
#include <stdlib.h>

void charu(int a[], int s, int m) {
    int i, k, t;
    for (i = s + 1; i <= s + m - 1; i++) {
        t = a[i];
        k = i - 1;
        while (t > a[k]) {
            a[k + 1] = a[k];
            k--;
            if (k == s - 1) break;
        }
        a[k + 1] = t;
    }
}

int main() {
    int a[100], n, i, s, m;
    scanf("%d", &n);
    m = n;
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d%d", &s, &m);
    charu(a, s, m);
    for (i = 0; i < n; i++) printf("%d ", a[i]);
    return 0;
}
