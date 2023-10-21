#include <stdio.h>
#include <stdlib.h>

int main() {
    float  c, n = 0;
    double i = 1;
    scanf("%f", &c);
    while (n < c) {
        n = n + 1 / (i + 1);
        i++;
    }
    printf("%.0lf", i - 1);
    return 0;
}
