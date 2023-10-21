#include <stdio.h>
#include <stdlib.h>

int main() {
    int a;
    scanf("%d", &a);
    if (a >= 90)
        printf("A\n");
    else if (a >= 80)
        printf("B\n");
    else if (a >= 70)
        printf("C\n");
    else if (a >= 60)
        printf("D\n");
    else
        printf("E\n");
    return 0;
}
