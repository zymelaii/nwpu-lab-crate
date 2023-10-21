#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, i, j;
    scanf("%d", &n);
    int a[n][n];
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++) scanf("%d", &a[i][j]);
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) printf("  ");
        for (j = i; j < n; j++) printf("%d ", a[i][j]);
        printf("\n");
    }
    return 0;
}
