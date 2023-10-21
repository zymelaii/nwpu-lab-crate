#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char a[100], b[100];
    gets(a);
    gets(b);
    strncat(a, b, 100);
    puts(a);
    return 0;
}
