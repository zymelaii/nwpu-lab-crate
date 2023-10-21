#include <stdio.h>

int main() {
    int n, a, b, c, d, m;
    scanf("%d", &n);
    a = n % 10;
    b = (n / 10) % 10;
    c = (n / 100) % 10;
    d = n / 1000;
    a = (a + 5) % 10;
    b = (b + 5) % 10;
    c = (c + 5) % 10;
    d = (d + 5) % 10;
    m = 1000 * a + 100 * b + 10 * c + d;
    printf("%d", m);
    return 0;
}
