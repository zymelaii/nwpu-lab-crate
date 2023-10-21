#include <stdio.h>
#include <stdlib.h>

int main() {
    char A[80] = {0}, *p, x;
    int  i;
    p = A;
    gets(A);
    scanf("%c%d", &x, &i);
    for (; *p != '\0'; p++)
        ;
    for (; p > A + i; p--) *p = *(p - 1);
    *(A + i) = x;
    puts(A);
    return 0;
}
