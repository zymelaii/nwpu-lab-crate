#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char s[100][100], a[100];
    int  i, j, n;
    scanf("%d", &n);
    for (i = 0; i < n; i++) scanf("%s", s[i]);
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (strlen(s[i]) > strlen(s[i + 1]))
                strcpy(a, s[i]), strcpy(s[i], s[i + 1]), strcpy(s[i + 1], a);
    if (n % 2 == 0) {
        for (i = 0; i < n - 1; i = i + 2) printf("%s ", s[i]);
        printf("%s ", s[n - 1]);
        for (i = i - 3; i > 0; i = i - 2) printf("%s ", s[i]);
    } else {
        for (i = 0; i < n - 1; i = i + 2) printf("%s ", s[i]);
        printf("%s ", s[n - 1]);
        for (i = i - 1; i > 0; i = i - 2) printf("%s ", s[i]);
    }
    return 0;
}
