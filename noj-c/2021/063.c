#include <stdio.h>

void getbit(int n, int k) {
    int i;
    for (i = 2; i <= k; i++) n = n / 2;
    printf("%d", n % 2);
}

int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    getbit(n, k);
    return 0;
}
