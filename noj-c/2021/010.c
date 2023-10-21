#include <stdio.h>

int fun(int a) {
    if (a == 1)
        return 1;
    else
        return fun(a / 2) + 1;
}

int main() {
    int a, b[100], i = 0, j;
    while (1) {
        scanf("%d", &a);
        if (a == 0) break;
        b[i] = fun(a);
        i++;
    }
    for (j = 0; j < i; j++) { printf("%d\n", b[j]); }
    return 0;
}
