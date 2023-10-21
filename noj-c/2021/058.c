#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, c, s;
    scanf("%d%d%d", &a, &b, &c);
    for (s = 10; s <= 100; s++) {
        if ((s - a) % 3 == 0 && (s - b) % 5 == 0 && (s - c) % 7 == 0) {
            printf("%d", s);
            break;
        };
    };
    if (s == 101) printf("-1");
    return 0;
}
