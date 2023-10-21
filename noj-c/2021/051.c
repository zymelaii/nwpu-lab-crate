#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char s[1000], b[1000][1000];
    int  i, n, j, h = 0, m;
    gets(s);
    n = strlen(s);
    for (i = 0; i < n; i++) {
        if ((s[i] > 64 && s[i] < 91) || (s[i] > 96 && s[i] < 123)) {
            m = 0;
            for (j = i;; j++) {
                if ((s[j] > 64 && s[j] < 91) || (s[j] > 96 && s[j] < 123)) {
                    b[h][m] = s[j];
                    m++;
                    i++;
                } else {
                    b[h][m] = ' ';
                    h++;
                    break;
                }
            }
            h++;
        }
    }
    for (i = h - 2; i >= 0; i--) printf("%s", b[i]);
    return 0;
}
