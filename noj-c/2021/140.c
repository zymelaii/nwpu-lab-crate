#include <stdio.h>
#include <string.h>

void Left(char str[], int n, char dest[]) {
    int i;
    for (i = 0; i < n; i++) dest[i] = str[i];
}

void Right(char str[], int n, char dest[]) {
    int i, m, j = 0;
    m = strlen(str);
    for (i = m - n; i <= m; i++) {
        dest[j] = str[i];
        j++;
    }
}

void Mid(char str[], int loc, int n, char dest[]) {
    int i, j = 0;
    for (i = loc; i < loc + n; i++) {
        dest[j] = str[i];
        j++;
    }
}

int main() {
    int  n, i, loc;
    char str[81], dest[81];
    gets(str);
    scanf("%d%d", &n, &loc);
    Left(str, n, dest);
    for (i = 0; i < n; i++) printf("%c", dest[i]);
    printf("\n");
    Right(str, n, dest);
    puts(dest);
    Mid(str, loc, n, dest);
    puts(dest);
    return 0;
}
