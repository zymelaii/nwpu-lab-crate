#include <stdio.h>
#include <stdlib.h>

int main() {
    int a[100], i, j, n, k;
    for (i = 0; i < 5; i++) scanf("%d", &a[i]);
    if (a[1] + a[0] == a[2] && a[2] + a[3] == a[4]) {
        for (i = 5; i < 10; i++) a[i] = a[i - 1] + a[i - 2];
        for (i = 5; i < 10; i++) printf("%d ", a[i]);
        return 0;
    }
    if (a[1] - a[0] == a[2] - a[1] && a[4] - a[3] == a[2] - a[1]) {
        k = a[1] - a[0];
        n = a[4];
        for (i = 0; i < 5; i++) n = n + k, printf("%d ", n);
        return 0;
    }
    if (a[1] / a[0] == a[2] / a[1] && a[4] / a[3] == a[2] / a[1]) {
        k = a[1] / a[0];
        n = a[4];
        for (i = 0; i < 5; i++) n = n * k, printf("%d ", n);
        return 0;
    }
    return 0;
}
