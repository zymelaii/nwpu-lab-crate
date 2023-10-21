#include <stdio.h>
#include <stdlib.h>

int main() {
    int A[100], i = 0, j, k, female = 0, male = 1, x;
    for (;; i++) {
        scanf("%d", &A[i]);
        if (A[i] == -1) break;
    }
    for (j = 0; j < i; j++) {
        female = 0, male = 1;
        for (k = 1; k < A[j]; k++) {
            x      = female;
            female = male;
            male   = x + male + 1;
        }
        printf("%d %d\n", male, female + male + 1);
    }
    return 0;
}
