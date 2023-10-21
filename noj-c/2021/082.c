#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, c, i, j, m;
    scanf("%d%d", &a, &b);
    if (a > b) {
        c = b;
        b = a;
        a = c;
    }
    for (i = a; i <= b; i++) {
        if (i == 2)
            printf("2");
        else {
            for (j = 2; j < i; j++) {
                m = i % j;
                if (m == 0) goto E;
            }
            printf("%d ", i);
        }
E:
        continue;
    }
    return 0;
}
