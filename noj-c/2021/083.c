#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int  a[100], i, n, left = 0, right = 0;
    char c[5];
    scanf("%s%d", c, &n);
    for (i = 0; i < n; i++) scanf("%d", &a[i]);
    for (i = 0; i < n; i++)
        if (a[i] == 0) left++;
    right = n - left;
    n     = right - left;
    if (n >= 0)
        n = n % 4;
    else
        n = 4 - (-n) % 4;
    if (c[0] == 'E') {
        if (n == 0) printf("East");
        if (n == 1) printf("South");
        if (n == 2) printf("West");
        if (n == 3) printf("North");
    }
    if (c[0] == 'S') {
        if (n == 3) printf("East");
        if (n == 0) printf("South");
        if (n == 1) printf("West");
        if (n == 2) printf("North");
    }
    if (c[0] == 'W') {
        if (n == 2) printf("East");
        if (n == 3) printf("South");
        if (n == 0) printf("West");
        if (n == 1) printf("North");
    }
    if (c[0] == 'N') {
        if (n == 1) printf("East");
        if (n == 2) printf("South");
        if (n == 3) printf("West");
        if (n == 0) printf("North");
    }
    return 0;
}
