#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m = 7, b;
    scanf("%d", &n);
    while (m <= n) {
        if ((m % 7 != 0) && ((m - 7) % 10 != 0))
            b = b + 1;
        else
            printf("%d ", m);
        m = m + 1;
    };
    return 0;
}
