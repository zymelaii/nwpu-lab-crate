#include <stdio.h>
#include <string.h>

int main() {
    char a[80], b[80], c[80];
    int  i, d = 0, x = 0, s = 0, k = 0, q = 0, n;
    gets(a);
    gets(b);
    gets(c);
    n = strlen(a);
    for (i = 0; i < n; i++)
        if (a[i] < 91 && a[i] > 64)
            d++;
        else if (a[i] > 96 && a[i] < 123)
            x++;
        else if (a[i] > 47 && a[i] < 58)
            s++;
        else if (a[i] == 32)
            k++;
        else
            q++;
    n = strlen(b);
    for (i = 0; i < n; i++)
        if (b[i] < 91 && b[i] > 64)
            d++;
        else if (b[i] > 96 && b[i] < 123)
            x++;
        else if (b[i] > 47 && b[i] < 58)
            s++;
        else if (b[i] == 32)
            k++;
        else
            q++;
    n = strlen(c);
    for (i = 0; i < n; i++)
        if (c[i] < 91 && c[i] > 64)
            d++;
        else if (c[i] > 96 && c[i] < 123)
            x++;
        else if (c[i] > 47 && c[i] < 58)
            s++;
        else if (c[i] == 32)
            k++;
        else
            q++;
    printf("%d %d %d %d %d", d, x, s, k, q);
    return 0;
}
