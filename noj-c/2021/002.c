#include <stdio.h>

int doubi(int n, int m) {
    n = n + m;
    n = n % 100;
    return n;
}

int main() {
    int t, i, a[100], n, m;
    scanf("%d", &t);
    for (i = 0; i <= (t - 1); i++) {
        scanf("%d%d", &n, &m);
        a[i] = doubi(n, m);
    }
    for (i = 0; i <= (t - 1); i++) printf("%d\n", a[i]);
    return 0;
}
