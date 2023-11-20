#include <stdio.h>
#include <string.h>

int main() {
    unsigned char IN[65536] = {0}, *p = IN;
    int           i = 0;
    fseek(stdin, 0, SEEK_END);
    int len = ftell(stdin);
    rewind(stdin);
    fread(IN, 1, len, stdin);
    while (len-- > 0) {
        printf("%02x", *p++);
        if (++i < 16) {
            printf(" ");
        } else {
            printf("\n");
            i = 0;
        }
    }
    return 0;
}
