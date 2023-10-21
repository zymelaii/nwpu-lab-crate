#include <stdio.h>
#include <stdlib.h>
#define M 70
int start[M], targe[M];

long long f(int *p, int k, int fina) {
    if (k == 0) return 0;
    if (p[k] == fina) return f(p, k - 1, fina);
    return f(p, k - 1, 6 - fina - p[k]) + (1LL << (k - 1));
}

int main() {
    long long ans;
    int       n;
    while (scanf("%d", &n), n) {
        int i;
        for (i = 1; i <= n; i++) scanf("%d", &start[i]);
        for (i = 1; i <= n; i++) scanf("%d", &targe[i]);
        int c = n;
        for (; c >= 1 && start[c] == targe[c]; c--)
            ;
        if (c == 0) {
            printf("0\n");
            continue;
        }
        int other = 6 - start[c] - targe[c];
        ans       = f(start, c - 1, other) + f(targe, c - 1, other) + 1;
        printf("%lld\n", ans);
    }
    return 0;
}
