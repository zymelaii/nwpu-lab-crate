#include <stdio.h>
#include <stdlib.h>

int main() {
    int t, n = 1, m = 0, a;
    scanf("%d", &t);
    while (t > 0) {
        a = m;
        m = 3 * n + 2 * m;
        n = a;
        t = t - 1;
    }
    printf("%d %d", n, m);
    return 0;
}
