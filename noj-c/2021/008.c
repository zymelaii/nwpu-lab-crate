#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int  i, n, t, j;
    char s[100][100];
    for (i = 0;; i++) {
        gets(s[i]);
        if (s[i][0] == '#') break;
    }
    n = i;
    for (i = 0; i < n; i++) {
        t = 0;
        for (j = 0; j < strlen(s[i]); j++)
            if (s[i][j] == 32)
                t = t;
            else
                t = t + (j + 1) * (s[i][j] - 64);
        printf("%d\n", t);
    }
    return 0;
}
