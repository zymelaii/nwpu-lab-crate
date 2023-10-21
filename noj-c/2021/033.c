#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void BubbleSort(double A[], int s, int m) {
    int    i, j;
    double k;
    for (j = s; j < m + s - 1; j++)
        for (i = s; i < m + s - 1; i++)
            if (A[i] > A[i + 1]) {
                k        = A[i];
                A[i]     = A[i + 1];
                A[i + 1] = k;
            }
}

int main() {
    int    n, i;
    double X[10000], Y[10000], s1 = 0, s2 = 0, s = 0;
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        scanf("%lf%lf", &X[i], &Y[i]);
        s2 += X[i];
    }
    BubbleSort(X, 0, n);
    for (i = 0; i < n; i++) s1 += X[i] - i * 1.0;
    s1 = (int)((s1 / n) + 0.5);
    s2 = (int)((s2 / n) + 0.5);
    for (i = 0; i < n; i++) s = s + fabs(X[i] - i - s1) + fabs(Y[i] - s2);
    printf("%.0lf", s);
    return 0;
}
