#include <stdio.h>
#include <stdlib.h>

int main() {
    int a[100], j = 1, i, n, m;
    scanf("%d%d", &n, &m);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < n; i++)
        if (a[i] > a[m]) j++;
    printf("%d", j++);
    return 0;
}
