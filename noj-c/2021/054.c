#include <stdio.h>
#include <stdlib.h>

int main() {
    int  a, b, c, d, e, f, n, h = 0;
    char s;
    scanf("%d/%d%c%d/%d", &a, &b, &s, &c, &d);
    if (s == '+') {
        e = a * d + b * c;
        f = b * d;
        n = f;
        if (e == 0) {
            printf("0");
            return 0;
        }
        if (e < 0) e = -e, h = 1;
        if (f % e == 0) {
            f = f / e;
            e = e / (f / n);
        }
        if (h == 0)
            printf("%d/%d", e, f);
        else
            printf("-%d/%d", e, f);
    }
    if (s == '-') {
        e = a * d - b * c;
        f = b * d;
        n = f;
        if (e == 0) {
            printf("0");
            return 0;
        }
        if (e < 0) e = -e, h = 1;
        if (f % e == 0) {
            f = f / e;
            e = e / (f / n);
        }
        if (h == 0)
            printf("%d/%d", e, f);
        else
            printf("-%d/%d", e, f);
    }
    return 0;
}
