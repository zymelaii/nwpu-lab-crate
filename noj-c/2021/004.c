#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char   s[100], q[100];
    double a, b, c;
    int    n = 0, i;
    scanf("%lf%lf", &a, &b);
    c = a * b;
    sprintf(s, "%.0lf", c);
    for (i = 0; i < strlen(s); i++) { n = n + s[i] - 48; }
    while (n >= 10) {
        sprintf(q, "%d", n);
        n = 0;
        for (i = 0; i < strlen(q); i++) n = n + q[i] - 48;
    }
    printf("%d", n);
    return 0;
}
