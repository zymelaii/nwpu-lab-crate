#include <stdio.h>
#include <stdlib.h>

int main() {
    int a[100], j, i, n;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if (a[i] == a[j]) {
                a[i] = 0;
                a[j] = 0;
            }
    for (i = 0; i < n; i++)
        if (a[i] != 0) printf("%d ", a[i]);
    return 0;
}
