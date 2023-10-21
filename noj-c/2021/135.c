#include <stdio.h>
#include <stdlib.h>

int main() {
    char A[99];
    int  i = 0, j, a;
    while ((A[i] = getchar()) != '\n') i++;
    scanf("%d", &a);
    for (j = a; j < i; j++) printf("%c", A[j]);
    return 0;
}
