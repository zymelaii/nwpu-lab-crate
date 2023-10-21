#include <stdio.h>
#include <stdlib.h>

void SectionSort(int a[], int s, int m) {
    int i, j, k, t;
    for (i = s; i <= m; i++) {
        k = i;
        for (j = i + 1; j <= m; j++)
            if (a[j] > a[k]) k = j;
        if (i != k) {
            t    = a[i];
            a[i] = a[k];
            a[k] = t;
        }
    }
}

int main() {
    int n, a[100], i, s, m;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d%d", &s, &m);
    m = m + s - 1;
    SectionSort(a, s, m);
    for (i = 0; i < n; i++) printf("%d ", a[i]);
    return 0;
}
