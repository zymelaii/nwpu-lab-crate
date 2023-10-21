#include <stdio.h>
#include <stdlib.h>

int main() {
    int A[100], B[100], i, n;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &A[i]);
    for (i = 0; i < n; i++) scanf("%d", &B[i]);
    for (i = 0; i < n; i++) printf("%d ", A[i] + B[i]);
    return 0;
}
