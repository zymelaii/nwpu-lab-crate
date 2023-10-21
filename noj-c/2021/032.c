#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, i, s = 0;
    scanf("%d", &n);
    for (i = 0;; i++) {
        s = s + 10;
        if (s >= n) break;
        s = s - 5;
    }
    printf("%d", i + 1);
    return 0;
}
