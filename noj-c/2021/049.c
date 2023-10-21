#include <stdio.h>

int main() {
    int i, j, m, s = 0;
    scanf("%d", &j);
    for (m = 0; m < j; m++) {
        scanf("%d", &i);
        if (i == 0) { break; }
        if (i >= 100) {
            s = s + i / 100;
            i = i % 100;
        }
        if (i >= 50) {
            s = s + i / 50;
            i = i % 50;
        }
        if (i >= 10) {
            s = s + i / 10;
            i = i % 10;
        }
        if (i >= 5) {
            s = s + i / 5;
            i = i % 5;
        }
        if (i >= 2) {
            s = s + i / 2;
            i = i % 2;
        }
        if (i >= 1) { s++; }
    }
    printf("%d", s);
    return 0;
}
