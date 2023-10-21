#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void cheng(char a[], char b[]) {
    int i, j, ca, cb, *s;
    ca = strlen(a);
    cb = strlen(b);
    s  = (int *)malloc(sizeof(int) * (ca + cb));
    for (i = 0; i < ca + cb; i++) s[i] = 0;
    for (i = 0; i < ca; i++)
        for (j = 0; j < cb; j++) s[i + j + 1] += (a[i] - '0') * (b[j] - '0');
    for (i = ca + cb - 1; i >= 0; i--)
        if (s[i] >= 10) {
            s[i - 1] += s[i] / 10;
            s[i]     %= 10;
        }
    i = 0;
    while (s[i] == 0) i++;
    for (; i < ca + cb; i++) printf("%d", s[i]);
    printf("\n");
    free(s);
}

int main() {
    char a[100], b[100];
    gets(a), gets(b);
    if (a[0] == '-' && b[0] != '-') {
        printf("-");
        cheng(&a[1], b);
    } else if (a[0] == '-' && b[0] == '-') {
        cheng(&a[1], &b[1]);
    } else if (a[0] != '-' && b[0] == '-') {
        printf("-");
        cheng(a, &b[1]);
    } else
        cheng(a, b);
    return 0;
}
