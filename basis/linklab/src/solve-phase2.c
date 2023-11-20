// solve-phase2 < phase2.o

#include <stdio.h>
#include <string.h>
#include <malloc.h>

unsigned char *loadfile(FILE *fp, int *plen) {
    fseek(fp, 0, SEEK_END);
    *plen = ftell(fp);
    rewind(fp);
    unsigned char *buf = (unsigned char *)malloc(*plen);
    fread(buf, 1, *plen, fp);
    return buf;
}

int main(int argc, char *argv[]) {
    int            len_in;
    unsigned char *in = loadfile(stdin, &len_in);
    for (int i = 0; i < len_in; ++i) {
        if (*(unsigned int *)(in + i) == 0x90909090) {
            in[i + 0] = 0x68;
            in[i + 1] = 0x34;
            in[i + 2] = 0x94;
            in[i + 3] = 0x04;
            in[i + 4] = 0x08;
            in[i + 5] = 0xe8;
            in[i + 6] = 0xbb;
            in[i + 7] = 0xff;
            in[i + 8] = 0xff;
            in[i + 9] = 0xff;
            break;
        }
    }
    for (int i = 0; i < len_in; ++i) { putchar(in[i]); }
    free(in);
    return 0;
}
