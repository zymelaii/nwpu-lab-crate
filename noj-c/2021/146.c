#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main() {
    char a[5000], b[5000], i, j = 0, c[5000], p = 0, h, s;
    gets(a);
    for (i = 0; i < strlen(a); i++) {
        if (a[i] >= 48 && a[i] <= 57) b[j] = a[i], j++;
        if (a[i] >= 65 && a[i] <= 90) b[j] = a[i], j++;
        if (a[i] >= 97 && a[i] <= 122) b[j] = a[i], j++;
    }
    for (i = 1; i < strlen(b); i++) {
        h = 0;
        if (b[i] == b[i + 1]) {
            for (j = 1; i - j + 1 >= 0 && i + j < strlen(b); j++)
                if (b[i - j + 1] == b[i + j]
                    || abs(b[i - j + 1] - b[i + 1]) == 32)
                    h++;
                else
                    break;
        }
        if (h > p) {
            p = h;
            for (s = i - j + 2; s < i + j; s++) c[s - i + j - 2] = b[s];
        }
        h = 0;
        if (b[i - 1] == b[i + 1]) {
            for (j = 1; i - j >= 0 && i + j < strlen(b); j++)
                if (b[i - j] == b[i + j] || abs(b[i - j] - b[i + j]) == 32)
                    h++;
                else
                    break;
        }
        if (h > p) {
            p = h;
            for (s = i - j + 1; s < i + j; s++) c[s - i + j - 1] = b[s];
        }
    }
    for (i = 0; i < strlen(a); i++)
        if (a[i] == c[0]) break;
    for (j = strlen(a); j > 0; j--)
        if (a[j] == c[strlen(c) - 1]) break;
    for (s = i; s <= j; s++) printf("%c", a[s]);
    return 0;
}
