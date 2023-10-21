#include <stdio.h>
#include <stdlib.h>

void bijiao(char *a, char *b) {
    int i;
    for (i = 0; i <= 80; i++)
        if (a[i] != b[i]) {
            printf("%d", a[i] - b[i]);
            break;
        }
}

int main() {
    char a[80], b[80];
    gets(a);
    gets(b);
    bijiao(a, b);
    return 0;
}
