#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, i, j, k = 0, t, A[100];
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &A[i]);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
            if (A[j] > A[j + 1]) {
                t        = A[j];
                A[j]     = A[j + 1];
                A[j + 1] = t;
                k++;
            }
    }
    printf("%d", k);
    return 0;
}
