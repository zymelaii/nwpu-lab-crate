#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    double A[100];
    int    i = 0, j = 0, k = 0, l = 0, sum = 0;
    while (1) {
        scanf("%lf", &A[i]);
        if (A[i] == 0) break;
        i++;
    }
    for (j = 0; j < i; j++) {
        if (A[j] == 2)
            printf("1\n");
        else {
            int    B[10000], m = 1, number = 0;
            double n;
            B[0] = 2;
            for (k = 3; k <= A[j]; k += 2) {
                n = (double)k;
                for (l = 2; l <= sqrt(n); l++) {
                    if (k % l == 0) goto ai;
                }
                B[m] = k;
                m++;
ai:;
            }
            for (k = 0; k < m; k++) {
                sum = 0;
                for (l = k; l < m; l++) {
                    sum += B[l];
                    if (sum == A[j]) {
                        number++;
                        break;
                    }
                }
            }
            printf("%d\n", number);
        }
    }
    return 0;
}
