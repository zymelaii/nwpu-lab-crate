#include <stdio.h>
#include <stdlib.h>

int main() {
    double s, n, m, a, b;
    n = 1;
    s = 0;
    m = 2;
    a = 1;
    while (a <= 20) {
        s = s + m / n;
        b = m;
        m = n + m;
        n = b;
        a = a + 1;
    }
    printf("%.6lf", s);
    return 0;
}
