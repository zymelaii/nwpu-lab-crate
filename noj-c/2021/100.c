#include <stdio.h>
#include <stdlib.h>

int main() {
    int a, b, c, d, n;
    scanf("%d%d", &a, &b);
    if (b > a) {
        c = a;
        a = b;
        b = c;
    };
    n = b + 1;
    while (n > b && n < a) {
        d     = n - 1;
        int e = 0;
        while (d > 1) {
            if ((n % d) == 0) e = e + 1;
            d = d - 1;
        };
        if (e == 0) printf("%d ", n);
        n = n + 1;
    };
    return 0;
}
