#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, n, c;
    a = 2;
    while (a < 1000) {
        b = a - 1;
        n = 0;
        while (b > 0) {
            if (a % b == 0) n = n + b;
            b = b - 1;
        };
        if (a == n) {
            b = 0;
            n = 0;
            do {
                b = b + 1;
                c = a % b;
            } while (c != 0);
            printf("%d=%d", a, b);
            b = b + 1;
            while (b < a) {
                if (a % b == 0) printf("+%d", b);
                b = b + 1;
            };
            printf("\n");
        };
        a = a + 1;
    };
    return 0;
}
