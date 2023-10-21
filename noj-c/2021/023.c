#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int  len[64], n, minlen, get;
bool b[64];

int cmp(const void *a, const void *b) {
    return *(int *)a < *(int *)b ? 1 : -1;
}

bool dfs(int nowlen, int nowget, int cnt) {
    if (cnt >= n) return false;
    if (get == nowget) return true;
    int  i;
    bool f = false;
    if (nowlen == 0) f = true;
    for (i = cnt; i < n; i++) {
        if (!b[i]) {
            if (len[i] + nowlen == minlen) {
                b[i] = true;
                if (dfs(0, nowget + 1, nowget)) return true;
                b[i] = false;
                return false;
            } else if (len[i] + nowlen < minlen) {
                b[i] = true;
                if (dfs(nowlen + len[i], nowget, i + 1)) return true;
                b[i] = false;
                if (f) return false;
                while (i + 1 < n && len[i] == len[i + 1]) i++;
            }
        }
    }
    return false;
}

int main() {
    int i, tollen;
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
                printf("%d\n", minlen);
                break;
            }
        }
    }
    return 0;
}
