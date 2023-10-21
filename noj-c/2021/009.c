#include <stdio.h>
#include <stdlib.h>

int main() {
    char   A[100000];
    int    n, i = 0, a = 0, b = 0, j;
    double x;
    while (1) {
        scanf("%c", &A[i]);
        if (A[i] == '\n') break;
        i++;
    }
    for (j = 0; j < i; j++) {
        if (A[j] == 'S') {
            printf("WA");
            goto OH;
        }
        if (A[j] == 'U') a++;
        if (A[j] == 'D') b++;
    }
    x = a * 1.0 / (a + b) * 1.0;
    if (x - 0.5 > 0.003 || x - 0.5 < -0.003)
        printf("Fail");
    else
        printf("%d/%d", a, a + b);
OH:
    return 0;
}
