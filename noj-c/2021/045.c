#include <stdio.h>
#include <stdlib.h>

int main() {
    char  s[1000][1000];
    FILE *p;
    int   i, j = 0, k = 0, h = 0, q = 0, e;
    p = fopen("DATA5610.TXT", "r");
    while (!feof(p)) {
        if (fgets(s[q], 999, p) == NULL) continue;
        q++;
    }
    e = q;
    for (q = 0; q < e; q++)
        for (i = 0; i < 1000; i++) {
            if (s[q][i] == 'i' && s[q][i + 1] == 'f') h++;
            if (s[q][i] == 'w' && s[q][i + 1] == 'h' && s[q][i + 2] == 'i'
                && s[q][i + 3] == 'l' && s[q][i + 4] == 'e')
                j++;
            if (s[q][i] == 'f' && s[q][i + 1] == 'o' && s[q][i + 2] == 'r') k++;
        }
    printf("%d %d %d", h, j, k);
    return 0;
}
