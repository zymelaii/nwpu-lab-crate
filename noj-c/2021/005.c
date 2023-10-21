#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int ack(int x, int y) {
    int n;
    if (x == 0) {
        n = y + 1;
        return n;
    } else if (y == 0)
        n = ack(x - 1, 1);
    else
        n = ack(x - 1, ack(x, y - 1));
    return n;
}

int main() {
    int m, b;
    scanf("%d%d", &m, &b);
    m = ack(m, b);
    printf("%d", m);
    return 0;
}
