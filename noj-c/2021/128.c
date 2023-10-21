#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int a, b, c;
    scanf("%d%d%d", &a, &b, &c);
    if (a < b) {
        a = b;
        if (a < c) { a = c; }
    } else {
        if (a < c) { a = c; }
    }
    printf("%d", a);
    return 0;
}
