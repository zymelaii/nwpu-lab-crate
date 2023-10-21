#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char s[1000], b[1000];
    int  i, n, h = 0;
    gets(s);
    n = strlen(s);
    for (i = 0; i < n; i++) {
        if (s[i] == 121 && s[i + 1] == 111 && s[i + 2] == 117) {
            b[h]     = 119;
            b[h + 1] = 101;
            h        = h + 2;
            i        = i + 2;
        } else {
            b[h] = s[i];
            h++;
        };
    }
    for (i = 0; i < h; i++) printf("%c", b[i]);
    return 0;
}
