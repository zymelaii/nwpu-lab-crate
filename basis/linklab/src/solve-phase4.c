// solve-phase4 <student-id> < phase4.o

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

const int iTABLE  = 0x90;
const int iRODATA = 0x1b0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "error: require student-id\n");
        return -1;
    }
    const char    *stuid = argv[1];
    int            len_in;
    unsigned char *in        = loadfile(stdin, &len_in);
    char           index[11] = {0};
    for (int i = 0; i < len_in; ++i) {
        if (*(unsigned int *)(in + i) == 0x83e58955) {
            index[0] = in[i + 20];
            index[1] = in[i + 21];
            index[2] = in[i + 22];
            index[3] = in[i + 23];
            index[4] = in[i + 27];
            index[5] = in[i + 28];
            index[6] = in[i + 29];
            index[7] = in[i + 30];
            index[8] = in[i + 35];
            index[9] = in[i + 36];
            break;
        }
    }
    fprintf(stderr, "info: parsed cookie: %s\n", index);
    char           table[27] = {0};
    unsigned char *pbtable   = in + iTABLE;
    for (int i = 0; i < 26; ++i) {
        while ((*(int *)pbtable & 0x00ffffff) != 0x00d745c6) { ++pbtable; }
        table[i] = pbtable[3];
        ++pbtable;
    }
    fprintf(stderr, "info: parsed table: %s\n", table);
    int offset[26], ioffset[26];
    for (int i = 0; i < 26; ++i) {
        offset[i]  = *(in + iRODATA + i * 4);
        ioffset[i] = i;
    }
    for (int i = 0; i < 10; ++i) {
        int j = -1;
        while (table[++j] != stuid[i])
            ;
        ioffset[index[i] - 'A'] = j;
    }
    for (int i = 0; i < 26; ++i) {
        *(in + iRODATA + i * 4) = offset[ioffset[i]];
    }
    for (int i = 0; i < len_in; putchar(in[i++])) {}
    free(in);
    return 0;
}
