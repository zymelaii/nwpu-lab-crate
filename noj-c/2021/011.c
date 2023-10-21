#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int a[100], b[100], i, j, n, t = 0;
    for (i = 0;; i++) {
        scanf("%d", &a[i]);
        if (a[i] == 0) break;
    }
    n = i;
    for (i = 0; i < n; i++) b[i] = 2 * a[i];
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            if (a[i] == b[j]) t++;
    printf("%d", t);
    return 0;
}
