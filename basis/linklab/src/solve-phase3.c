// solve-phase3 <student-id> < phase3.o

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
    if (argc != 2) {
        fprintf(stderr, "error: require student-id\n");
        return -1;
    }
    const char    *stuid = argv[1];
    int            len_in;
    unsigned char *in        = loadfile(stdin, &len_in);
    char           index[11] = {0}, vname[11] = {0};
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
    for (int i = 0; i < len_in; ++i) {
        int j = 0;
        for (; (char)in[i + j] == "phase3.c\0"[j] && j < 9; ++j)
            ;
        if (j == 9) {
            i += 9;
            for (j = 0; j < 10; vname[j] = in[i + j], ++j)
                ;
            fprintf(stderr, "info: parsed table name: %s\n", vname);
            break;
        }
    }
    char table[256] = {0};
    for (int i = 0; i < 10; ++i) { table[(unsigned char)index[i]] = stuid[i]; }
    char *source = (char *)malloc(4096), *psrc = source;
    sprintf(psrc, "char %s[256] = {\n", vname);
    psrc += strlen(psrc);
    for (int i = 0; i < 16; ++i) {
        int offset = i * 16;
        *psrc++    = '\t';
        for (int j = 0; j < 16; ++j) {
            sprintf(psrc, "0x%02x, ", table[offset + j]);
            psrc += strlen(psrc);
        }
        *psrc++ = '\n';
    }
    psrc[0] = '}';
    psrc[1] = ';';
    psrc[2] = '\0';
    puts(source);
    free(source);
    free(in);
    return 0;
}
