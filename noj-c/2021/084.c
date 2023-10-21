#include <stdio.h>
#include <stdlib.h>

void BubbleSort(int A[], int s, int m) {
    int i, j, k;
    for (j = s; j < m + s - 1; j++) {
        for (i = s; i < m + s - 1; i++) {
            if (A[i] < A[i + 1]) {
                k        = A[i];
                A[i]     = A[i + 1];
                A[i + 1] = k;
            }
        }
    }
}

int main() {
    int a, b, c, A[100], d;
    scanf("%d", &c);
    for (d = 0; d < c; d++) scanf("%d", &A[d]);
    scanf("%d%d", &a, &b);
    BubbleSort(A, a, b);
    for (d = 0; d < c; d++) printf("%d ", A[d]);
    return 0;
}
