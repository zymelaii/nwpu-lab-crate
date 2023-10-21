#include <stdio.h>

int jie(int n) {
    if (n > 1) return jie(n - 1) * n;
    return 1;
}

int zuhe(int n, int m) {
    int i;
    i = jie(n - m);
    n = jie(n);
    m = jie(m);
    n = n / (m * i);
    return n;
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    printf("%d", zuhe(n, m));
    return 0;
}
