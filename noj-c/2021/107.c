#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char  a[100][1000], b[100][1000];
    FILE *p1, *p2;
    int   i = 0, j = 0, n;
    p1 = fopen("DATA5613.TXT", "r");
    p2 = fopen("DATA5613.CPP", "rb");
    while (!feof(p1)) {
        if (fgets(a[i], 99, p1) == NULL) continue;
        i++;
    }
    i = 0;
    while (!feof(p2)) {
        if (fgets(b[i], 99, p2) == NULL) continue;
        i++;
    }
    n = i;
    for (i = 0; i < n; i++)
        for (j = 0; j < strlen(a[i]) - 1; j++)
            if (a[i][j] != b[i][j]) {
                printf("%d %d\n", i + 1, j + 1);
                break;
            };
    return 0;
}
