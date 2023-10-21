#include <stdio.h>
#include <stdlib.h>

int main() {
    int A[100][100], n, i, j;
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) scanf("%d", &A[i][j]);
    }
    int x = 0;
    for (j = 0; j < n; j++) {
        for (i = 0; i < n - x; i++) printf("%d ", A[j][i]);
        for (i = 0; i < x; i++) printf("  ");
        printf("\n");
        x++;
    }
    return 0;
}
