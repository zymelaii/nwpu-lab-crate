#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int N, i, n, j = 0;
    scanf("%d", &N);
    for (i = 2; i < N + 1; i++) {
        if ((N + 1) % i == 0) j++;
    }
    printf("%d", j / 2);
    return 0;
}
