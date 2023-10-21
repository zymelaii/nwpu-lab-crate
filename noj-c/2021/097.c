#include <stdio.h>
#include <stdlib.h>

int main() {
    int y, m, d;
    scanf("%d-%d-%d", &y, &m, &d);
    switch (m) {
        case 12:
            d = 30 + d;
        case 11:
            d = 31 + d;
        case 10:
            d = 30 + d;
        case 9:
            d = 31 + d;
        case 8:
            d = 31 + d;
        case 7:
            d = 30 + d;
        case 6:
            d = 31 + d;
        case 5:
            d = 30 + d;
        case 4:
            d = 31 + d;
        case 3:
            d = 28 + d;
        case 2:
            d = 31 + d;
        default:
            d = d;
    }
    printf("%d", d);
    return 0;
}
