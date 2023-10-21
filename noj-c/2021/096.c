#include <stdio.h>
#include <stdlib.h>

int main() {
    int y, m, d, days, i, sum = 0;
    scanf("%d-%d-%d", &y, &m, &d);
    for (i = 1; i < m; i++) {
        switch (i) {
            case 2:
                days = 28;
                if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) days++;
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                days = 30;
                break;
            default:
                days = 31;
        }
        sum += days;
    }
    sum = sum + d;
    printf("%d\n", sum);
    return 0;
}
