#include <stdio.h>

int main() {
    int x1, y1, x2, y2, x3, y3;
    while (~scanf("%d%d%d%d%d%d", &x1, &y1, &x2, &y2, &x3, &y3)
           && (x1 + y1 + x2 + y2 + x3 + y3)) {
        if ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1) > 0) {
            printf("0\n");
            return 0;
        } else {
            printf("1\n");
            return 0;
        }
    }
    return 0;
}
