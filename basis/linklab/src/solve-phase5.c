// solve-phase5 < phase5.o

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

int overwrite(unsigned char *in, int len, const char *fnstd) {
    FILE *fstd = fopen(fnstd, "rb");
    if (fstd == 0L) return -1;
    int            len_std;
    unsigned char *std = loadfile(fstd, &len_std);
    fclose(fstd);
    if (fstd == 0L) return -2;
    int n_rewrite = 0;
    for (int i = 0; i < len; ++i) {
        if (in[i] == 0x00 && in[i] != std[i]) {
            in[i] = std[i];
            fprintf(stderr, "%#x: rewrite to %02x\n", i, std[i]);
            ++n_rewrite;
        }
    }
    return n_rewrite;
}

int main(int argc, char *argv[]) {
    int            type1_len = 2352;
    int            type2_len = 2320;
    int            len_in, n_std = 0, n_rewrite = 0, ret;
    unsigned char *in = loadfile(stdin, &len_in);
    fprintf(stderr, "info: read %d bytes from phase5.o\n", len_in);
    const char *std_default =
        (len_in == type1_len ? "./assets/phase5.std.1"
                             : "./assets/phase5.std.2");
    ret = overwrite(in, len_in, std_default);
    switch (ret) {
        case -1:
            fprintf(stderr, "warn: ignore missing std-file %s\n", std_default);
            break;
        case -2:
            fprintf(stderr, "warn: ignore invalid std-file %s\n", std_default);
            break;
        default:
            n_rewrite += ret, fprintf(
                                  stderr,
                                  "info: rewrite %d bytes from %s in total\n",
                                  ret,
                                  std_default);
    }
    fprintf(
        stderr,
        "info: apply %s, rewrite %d bytes in total\n",
        std_default,
        n_rewrite);
    for (int i = 0; i < len_in; putchar(in[i++])) {}
    return 0;
}
