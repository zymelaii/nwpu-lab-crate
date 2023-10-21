#include <stdio.h>
#include <stdlib.h>

int main() {
    int a[100], b[100], n, i;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < n; i++) b[i] = a[i];
    for (i = 1; i < n; i = i + 2) printf("%d ", b[i]);
    return 0;
}
