#include <stdio.h>
#include <stdlib.h>

int main() {
    int A[100], n, a, b, j = 1, i;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%d", &A[i]);
    for (i = 0; i < n; i++) {
        a = A[i];
        for (j = i; j < n; j++)
            if (A[j] < A[i]) {
                if (a - A[j] == 1)
                    a = A[j];
                else {
                    printf("No");
                    goto ai;
                }
            }
    }
    printf("Yes");
ai:
    return 0;
}
