#include <stdio.h>
#include <stdlib.h>

int main() {
    int A[100][100], i, j, m, n, s = 0, t = 0;
    scanf("%d%d", &n, &m);
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= m; j++) { scanf("%d", &A[i][j]); }
    }
    for (i = 2; i < m; i++) s = s + A[1][i];
    for (i = 2; i < m; i++) s = s + A[n][i];
    for (i = 1; i <= n; i++) s = s + A[i][1];
    for (i = 1; i <= n; i++) s = s + A[i][m];
    for (i = 2; i < n; i++) {
        for (j = 2; j < m; j++) { t = t + A[i][j]; }
    }
    s = s - t;
    printf("%d", s);
    return 0;
}
