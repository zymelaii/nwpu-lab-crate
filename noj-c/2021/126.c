#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char c[80], b[80];
    int  n, i, s = 0, k, h = 0;
    gets(c);
    n = strlen(c);
    for (i = 0; i < n; i++) {
        if (c[i] <= 57 && c[i] >= 48) {
            s++;
            for (k = i;; k++) {
                if (c[k] <= 57 && c[k] >= 48) {
                    i++;
                    b[h] = c[k];
                    h++;
                } else {
                    b[h] = 32;
                    h++;
                    break;
                }
            }
        }
    }
    printf("%d\n", s);
    for (i = 0; i < h; i++) printf("%c", b[i]);
    return 0;
}
