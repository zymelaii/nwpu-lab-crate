#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int  i, j, sum, min, c, count, n, a, b;
    char s1[50], s2[50];
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        count = sum = 0;
        scanf("%s", s2);
        for (j = 0; j < 4; j++) {
            scanf("%d%d", &a, &b);
            if (b != 0) {
                sum += (a - 1) * 20 + b;
                count++;
            }
        }
        if (i == 0) {
            c = count, min = sum;
            strcpy(s1, s2);
        } else if (count > c || (count == c && sum < min)) {
            min = sum;
            c   = count;
            strcpy(s1, s2);
        }
    }
    printf("%s %d %d\n", s1, c, min);
    return 0;
}
