#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char a[1000], c;
    int  i, j, k = 0, h, n;
    gets(a);
    scanf("%c", &c);
    n = strlen(a);
    for (i = 0; i < n; i++) {
        h = 0;
        if (a[i] == c) {
            for (j = i;; j++) {
                if (a[j] == c)
                    h++, i++;
                else
                    break;
            }
        };
        if (h > k) k = h;
    }
    for (i = 0; i < k; i++) printf("%c", c);
    return 0;
}
