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
    int n, A[100], B[100], i, x = 0, y = 0;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &A[i]);
    for (i = 0; i < n; i++) scanf("%d", &B[i]);
    BubbleSort(A, 0, n);
    BubbleSort(B, 0, n);
    for (i = 0; i < n; i++) {
        if (A[i] > B[i]) x += 2;
        if (A[i] == B[i]) x++, y++;
        if (A[i] < B[i]) y += 2;
    }
    if (x > y) {
        printf("A");
        return 0;
    }
    if (x == y) {
        printf("=");
        return 0;
    }
    if (x < y) {
        printf("B");
        return 0;
    }
    return 0;
}
