#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char s[10][80], b[10][80];
    int  i, k, j;
    for (i = 0; i < 10; i++) scanf("%s", s[i]);
    for (i = 0; i < 10; i++) {
        k = 0;
        for (j = 0; j < 10; j++)
            if (strcmp(s[i], s[j]) > 0) k++;
        strcpy(b[k], s[i]);
    }
    for (i = 0; i < 10; i++) printf("%s ", b[i]);
    return 0;
}
