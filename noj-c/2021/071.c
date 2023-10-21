#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *p;
    long  biWidth, biHeight;
    p = fopen("DATA5611.BMP", "rb");
    if (p != NULL) {
        fseek(p, 18, SEEK_SET);
        fread(&biWidth, sizeof(long), 1, p);
        fread(&biHeight, sizeof(long), 1, p);
        printf("%ld %ld", biWidth, biHeight);
        fclose(p);
    }
    return 0;
}
