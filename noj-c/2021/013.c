#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAXNUM 100000

int  prime_number = 0;
int  prime_list[MAXNUM];
bool is_prime[MAXNUM];
int  ans[MAXNUM + 2];
int  dp[MAXNUM + 2];

void set_prime() {
    int i, j;
    memset(is_prime, 0, sizeof(is_prime));
    for (i = 2; i < MAXNUM; i++) {
        if (is_prime[i] == 0) {
            prime_list[prime_number++] = i;
            if (i >= MAXNUM / i) continue;
            for (j = i * i; j < MAXNUM; j += i) { is_prime[j] = 1; }
        }
    }
}

int main() {
    int i, j, k, o = 0, d[100];
    memset(dp, -1, sizeof(dp));
    set_prime();
    ans[0] = 0;
    dp[1]  = 0;
    for (i = 1; i <= MAXNUM; i++) {
        ans[i] = ans[i - 1] + dp[i];
        if (dp[i + 1] == -1 || dp[i + 1] > dp[i] + 1) { dp[i + 1] = dp[i] + 1; }
        for (j = 0; j < prime_number; j++) {
            if (i > MAXNUM / prime_list[j]) break;
            k = i * prime_list[j];
            if (dp[k] == -1 || dp[k] > dp[i] + 1) { dp[k] = dp[i] + 1; }
        }
    }
    while (scanf("%d%d", &i, &j) == 2 && (i || j)) {
        d[o] = ans[j] - ans[i - 1];
        o++;
    }
    for (i = 0; i < o; i++) printf("%d\n", d[i]);
}
