#include <stdio.h>

int move(int value, int n) {
    if (n == 0)
        return value;
    else if (n < 0) {
        n     = -n;
        value = (value << n) | (value >> (32 - n));
    } else
        value = (value >> n) | (value << (32 - n));
    return value;
}

int main() {
    int value, n;
    scanf("%d %d", &value, &n);
    value = move(value, n);
    printf("%d", value);
    return 0;
}
