#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char b[1000];

void deletechar(char s[], char a) {
    int n, i, j = 0, k;
    n = strlen(s);
    for (i = 0; i < n; i++) {
        if (s[i] != a)
            b[j] = s[i], j++;
        else {
            for (k = 0;; k++)
                if (s[i + k] != a) break;
            b[j] = s[i + k];
            i    = i + k;
            j++;
        }
    }
}

int main() {
    char s[1000], a;
    int  i, n;
    gets(s);
    n = strlen(s);
    scanf("%c", &a);
    deletechar(s, a);
    for (i = 0; i < n; i++) printf("%c", b[i]);
    return 0;
}
