#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void BubbleSort(double A[], int s, int m) {
    int    i, j;
    double k;
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
    int    n, z     = 0, i;
    double A[60], s = 0;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%lf", &A[i]);
    BubbleSort(A, 0, n);
    for (i = 0; i < n; i++) {
        z++;
        s = s + 2 * sqrt(A[i] * A[i] - 1);
        if (s >= 20) break;
    }
    printf("%d", z);
    return 0;
}
