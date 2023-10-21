#include <stdio.h>
#include <string.h>
#define MAXSIZE 1025

void Div(char *str1, char *str2, char *str3) {
    int  i1, i2, i, j, jj, tag, carry, cf, c[MAXSIZE];
    int  len1 = strlen(str1), len2 = strlen(str2), lend;
    char d[MAXSIZE];
    memset(c, 0, sizeof(c));
    memcpy(d, str1, len2);
    lend = len2;
    j    = 0;
    for (i1 = len2 - 1; i1 < len1; ++i1) {
        if (lend < len2) {
            d[lend] = str1[i1 + 1];
            c[j]    = 0;
            ++j;
            ++lend;
        } else if (lend == len2) {
            jj = 1;
            for (i = 0; i < lend; ++i) {
                if (d[i] > str2[i])
                    break;
                else if (d[i] < str2[i]) {
                    jj = 0;
                    break;
                }
            }
            if (jj == 0) {
                d[lend] = str1[i1 + 1];
                c[j]    = 0;
                ++j;
                ++lend;
                continue;
            }
        }
        if (jj == 1 || lend > len2) {
            cf = jj = 0;
            while (d[jj] <= '0' && jj < lend) ++jj;
            if (lend - jj > len2)
                cf = 1;
            else if (lend - jj < len2)
                cf = 0;
            else {
                i2 = 0;
                cf = 1;
                for (i = jj; i < lend; ++i) {
                    if (d[i] < str2[i2]) {
                        cf = 0;
                        break;
                    } else if (d[i] > str2[i2]) {
                        break;
                    }
                    ++i2;
                }
            }
            while (cf) {
                i2 = len2 - 1;
                cf = 0;
                for (i = lend - 1; i >= lend - len2; --i) {
                    d[i] = d[i] - str2[i2] + '0';
                    if (d[i] < '0') {
                        d[i]  = d[i] + 10;
                        carry = 1;
                        --d[i - 1];
                    } else
                        carry = 0;
                    --i2;
                }
                ++c[j];
                jj = 0;
                while (d[jj] <= '0' && jj < lend) ++jj;
                if (lend - jj > len2)
                    cf = 1;
                else if (lend - jj < len2)
                    cf = 0;
                else {
                    i2 = 0;
                    cf = 1;
                    for (i = jj; i < lend; ++i) {
                        if (d[i] < str2[i2]) {
                            cf = 0;
                            break;
                        } else if (d[i] > str2[i2]) {
                            break;
                        }
                        ++i2;
                    }
                }
            }
            jj = 0;
            while (d[jj] <= '0' && jj < lend) ++jj;
            for (i = 0; i < lend - jj; ++i) d[i] = d[i + jj];
            d[i] = str1[i1 + 1];
            lend = i + 1;
            ++j;
        }
    }
    i = tag = 0;
    while (c[i] == 0) ++i;
    for (; i < j; ++i, ++tag) str3[tag] = c[i] + '0';
    str3[tag] = '\0';
}

int main() {
    char a[110], b[110], c[110];
    scanf("%s%s", a, b);
    if (a[0] == '-' && b[0] != '-') printf("-");
    if (b[0] == '-' && a[0] != '-') printf("-");
    Div(a, b, c);
    printf("%s\n", c);
    return 0;
}
