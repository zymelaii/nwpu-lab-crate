#include <stdlib.h>
#include <stdio.h>

void QuickSort(int a[], int numsize) {
    int i = 0, j = numsize - 1;
    int val = a[0];
    if (numsize > 1) {
        while (i < j) {
            for (; j > i; j--)
                if (a[j] > val) {
                    a[i++] = a[j];
                    break;
                }
            for (; i < j; i++)
                if (a[i] < val) {
                    a[j--] = a[i];
                    break;
                }
        }
        a[i] = val;
        QuickSort(a, i);
        QuickSort(a + i + 1, numsize - i - 1);
    }
}

int main() {
    int n, b[99], a[99], s, m, i, l;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &b[i]);
    scanf("%d%d", &s, &m);
    for (i = s; i <= m; i++) a[i - s] = b[i];
    l = m - s + 1;
    QuickSort(a, l);
    for (i = s; i <= m; i++) b[i] = a[i - s];
    for (i = 0; i < n; i++) printf("%d ", b[i]);
    return 0;
}
