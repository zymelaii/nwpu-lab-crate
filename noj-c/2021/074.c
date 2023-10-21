#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int f(int n) {
    int s = 1;
    while (n > 1) {
        s = s * n;
        n--;
    }
    return s;
}

int main() {
    int i, j, k, s, n, t, p, q, y, x;
    scanf("%d", &n);
    if (n % 2 == 1) n--;
    for (i = 1;; i++)
        if (f(i) > n) break;
    s = i;
    for (i = 2; i < s; i++)
        if (f(i) == n) {
            printf("YES");
            return 0;
        }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            if (f(i) + f(j) == n) {
                printf("YES");
                return 0;
            }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                if (f(i) + f(j) + f(k) == n) {
                    printf("YES");
                    return 0;
                }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                for (t = k + 1; t < s; t++)
                    if (f(i) + f(j) + f(k) + f(t) == n) {
                        printf("YES");
                        return 0;
                    }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                for (t = k + 1; t < s; t++)
                    for (p = t + 1; p < s; p++)
                        if (f(i) + f(j) + f(k) + f(t) + f(p) == n) {
                            printf("YES");
                            return 0;
                        }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                for (t = k + 1; t < s; t++)
                    for (p = t + 1; p < s; p++)
                        for (q = p + 1; q < s; q++)
                            if (f(i) + f(j) + f(k) + f(t) + f(p) + f(q) == n) {
                                printf("YES");
                                return 0;
                            }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                for (t = k + 1; t < s; t++)
                    for (p = t + 1; p < s; p++)
                        for (q = p + 1; q < s; q++)
                            for (y = q + 1; y < s; y++)
                                if (f(i) + f(j) + f(k) + f(t) + f(p) + f(q)
                                        + f(y)
                                    == n) {
                                    printf("YES");
                                    return 0;
                                }
    for (i = 2; i < s; i++)
        for (j = i + 1; j < s; j++)
            for (k = j + 1; k < s; k++)
                for (t = k + 1; t < s; t++)
                    for (p = t + 1; p < s; p++)
                        for (q = p + 1; q < s; q++)
                            for (y = q + 1; y < s; y++)
                                for (x = y + 1; x < s; x++)
                                    if (f(i) + f(j) + f(k) + f(t) + f(p) + f(q)
                                            + f(y) + f(x)
                                        == n) {
                                        printf("YES");
                                        return 0;
                                    }
    printf("NO");
    return 0;
}
