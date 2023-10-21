#include <stdio.h>
#include <string.h>
int cardNum[20] = {0};
int isLucky[51] = {0};
int childLeft, count;

void pickChild(int n, int x) {
    int i, j;
    while (1) {
        for (i = 0; i < 20; ++i) {
            count = 0;
            for (j = 1; j <= n; ++j) {
                count += isLucky[j];
                if (count == cardNum[i]) {
                    isLucky[j] = 0;
                    if (x == --childLeft) return;
                    count = 0;
                }
            }
        }
    }
}

int main() {
    int n, x, i;
    scanf("%d%d", &n, &x);
    for (i = 0; i < 20; ++i) { scanf("%d", cardNum + i); }
    for (i = 1; i <= n; ++i) isLucky[i] = 1;
    childLeft = n;
    pickChild(n, x);
    for (i = 1; i <= n; i++)
        if (isLucky[i] == 1) printf("%d ", i);
    return 0;
}
