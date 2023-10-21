/**
 * @brief SP problom based on Floyd-Warshall algorithm
 * @author zymelaii
 * @date 2022-04-09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>

#define MAXN 256
typedef int Matrix[MAXN][MAXN];

int main(int argc, char const *argv[])
{
    int n, m;
    Matrix G; //@ graph
    Matrix D; //@ distance
    Matrix P; //@ path/transit

    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            scanf("%d", &G[i][j]);
            D[i][j] = G[i][j];
            P[i][j] = j; //@ let i->j == i->j + j->j
        }
    }

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j && D[i][j] > D[i][k] + D[k][j]) {
                    D[i][j] = D[i][k] + D[k][j];
                    P[i][j] = P[i][k]; //@ let i->j == i->k + k->... + ...->j + j->j
                }
            }
        }
    }

    scanf("%d", &m);
    while (m--) {
        int from, to;
        scanf("%d%d", &from, &to);
        for (; !(from == to && (printf("%d\n", to), true)); printf("%d\n", from), from = P[from][to]);
        //@ print shortest distance
        // printf("%d\n", D[from][to] >= 10000 ? 10000 : D[from][to]);
    }

    return 0;
}