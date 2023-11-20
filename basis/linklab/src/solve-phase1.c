// solve-phase1 <student-id> <index-string> < phase1.o

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
    if (argc != 3) {
        fprintf(stderr, "error: require student-id and index-string\n");
        return -1;
    }
    const char    *stuid  = argv[1];
    const char    *idxstr = argv[2];
    int            len_in;
    unsigned char *in = loadfile(stdin, &len_in);
    for (int i = 0; i < len_in; ++i) {
        if (strcmp((char *)(in + i), idxstr) == 0) {
            fprintf(stderr, "info: match index-string at %#x\n", i);
            strcpy((char *)(in + i), stuid);
            break;
        }
    }
    for (int i = 0; i < len_in; ++i) { putchar(in[i]); }
    free(in);
    return 0;
}
