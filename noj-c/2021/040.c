#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int a, n, i, j, x, y, z, A[10000], B[20], m = 1;
    scanf("%d", &n);
    for (i = 10000; i < 100000; i++) {
        a = i % n;
        if (a == 0) {
            A[m] = i;
            m++;
        }
    }
    for (j = 1; j <= m; j++) {
        x = A[j] / n;
        if (x < 1000)
a:
            continue;
        else {
            B[1]  = A[j] % 10;
            B[2]  = (A[j] / 10) % 10;
            B[3]  = (A[j] / 100) % 10;
            B[4]  = (A[j] / 1000) % 10;
            B[5]  = (A[j] / 10000) % 10;
            B[6]  = x % 10;
            B[7]  = (x / 10) % 10;
            B[8]  = (x / 100) % 10;
            B[9]  = (x / 1000) % 10;
            B[10] = (x / 10000) % 10;
            for (y = 1; y < 11; y++) {
                for (z = y + 1; z < 11; z++) {
                    if (B[y] - B[z] == 0) goto a;
                }
            }
        }
        if (x < 10000)
            printf("%d/0%d=%d\n", A[j], x, n);
        else
            printf("%d/%d=%d\n", A[j], x, n);
    }
    return 0;
}
