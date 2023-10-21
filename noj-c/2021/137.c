#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    char A[100], B[100];
    int  i = 0, j;
    while ((A[i] = getchar()) != '=') i++;
    for (j = i - 1; j > -1; j--) printf("%c", A[j]);
    return 0;
}
