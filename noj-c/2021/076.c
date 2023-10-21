#include <stdio.h>
#include <stdlib.h>

int main() {
    char a[10];
    int  n, b = 0, c[250] = {0}, i, j, x, flag = 0;
    c[249] = 1;
    scanf("%s%d", a, &n);
    for (i = 0; a[i] != '\0'; i++) {
        if (a[i] == '.') {
            x    = i;
            flag = 1;
            continue;
        }
        b = b * 10 + a[i] - '0';
    }
    x = i - x - 1;
    for (i = 1; i < n + 1; i++) {
        for (j = 0; j < 250; j++) c[j] *= b;
        for (j = 249; j >= 0; j--)
            if (c[j] >= 10) {
                c[j - 1] += c[j] / 10;
                c[j]     %= 10;
            }
    }
    if (flag == 0) {
        for (i = 0; c[i] == 0; i++)
            ;
        for (; i < 250; i++) printf("%d", c[i]);
    } else if (a[0] == '0') {
        flag = 0;
        printf("0");
        for (j = i; j < 250; j++)
            if (c[j] != 0) {
                flag = 1;
                break;
            }
        if (flag == 1) {
            printf(".");
            for (j = 249; c[j] == 0 && j >= i; j--)
                ;
            for (i = 250 - n * x; i <= j; i++) printf("%d", c[i]);
        }
    } else {
        flag = 0;
        for (i = 0; c[i] == 0; i++)
            ;
        for (; i < 250 - n * x; i++) printf("%d", c[i]);
        for (j = i; j < 250; j++)
            if (c[j] != 0) {
                flag = 1;
                break;
            }
        if (flag == 1) {
            printf(".");
            for (j = 249; c[j] == 0 && j >= i; j--)
                ;
            for (; i <= j; i++) printf("%d", c[i]);
        }
    }
    printf("\n");
    return 0;
}
