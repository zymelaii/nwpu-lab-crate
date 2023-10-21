#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int len[64], n, minlen, get;
int b[64];

int cmp(const void *a, const void *b) {
    return *(int *)a < *(int *)b ? 1 : -1;
}

int dfs(int nowlen, int nowget, int cnt) {
    if (cnt >= n) return 0;
    if (get == nowget) return 1;
    int i, f = 0;
    if (nowlen == 0) f = 1;
    for (i = cnt; i < n; i++) {
        if (len[i] + nowlen == minlen) {
            b[i] = 1;
            if (dfs(0, nowget + 1, nowget)) return 1;
            b[i] = 0;
            return 0;
        } else if (len[i] + nowlen < minlen) {
            b[i] = 1;
            if (dfs(nowlen + len[i], nowget, i + 1)) return 1;
            b[i] = 0;
            if (f) return 0;
            while (i + 1 < n && len[i] == len[i + 1]) i++;
        }
    }
    return 0;
}

int main() {
    int i, tollen, q = 0, c[100];
    while (scanf("%d", &n), n) {
        tollen = 0;
        int j  = 0, p;
        for (i = 0; i < n; i++) {
            scanf("%d", &p);
            if (p <= 50) {
                len[j]  = p;
                tollen += len[j];
                j++;
            }
        }
        n = j;
        if (n == 0) {
            printf("0\n");
            continue;
        }
        qsort(len, n, sizeof(int), cmp);
        for (minlen = len[0];; minlen++) {
            if (tollen % minlen) continue;
            memset(b, 0, sizeof(b));
            get = tollen / minlen;
            if (dfs(0, 0, 0)) {
                c[q] = minlen;
                q++;
                break;
            }
        }
    }
    for (i = 0; i < q; i++) printf("%d\n", c[i]);
    return 0;
}
