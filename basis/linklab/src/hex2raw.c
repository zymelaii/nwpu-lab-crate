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

int check(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')
        || (ch >= 'A' && ch <= 'F');
}

unsigned char c2tobyte(unsigned char c1, unsigned char c2) {
    if (c1 >= 'a' && c1 <= 'f') {
        c1 = c1 - 'a' + 10;
    } else if (c1 >= 'A' && c1 <= 'F') {
        c1 = c1 - 'A' + 10;
    } else {
        c1 = c1 - '0';
    }
    if (c2 >= 'a' && c2 <= 'f') {
        c2 = c2 - 'a' + 10;
    } else if (c2 >= 'A' && c2 <= 'F') {
        c2 = c2 - 'A' + 10;
    } else {
        c2 = c2 - '0';
    }
    return c1 * 16 + c2;
}

int main(int argc, char *argv[]) {
    int            len = 0, nbytes = 0;
    unsigned char *in  = loadfile(stdin, &len);
    int            row = 0, col = 0;
    for (int i = 0; i < len; ++i) {
        if (in[i] == ' ') {
            ++col;
            continue;
        }
        if (in[i] == '\n') {
            ++row;
            col = 0;
            continue;
        }
        if (!check(in[i])) {
            fprintf(
                stderr,
                "error: <%d:%d>: match invalid char '%c' (0x%02x)\n",
                row + 1,
                col + 1,
                in[i],
                in[i]);
            free(in);
            return -1;
        } else {
            ++col;
            if (i + 1 == len) {
                fprintf(
                    stderr,
                    "error: <%d:%d>: reach EOF while matching byte\n",
                    row + 1,
                    col + 1);
                free(in);
                return -2;
            } else if (!check(in[i + 1])) {
                fprintf(
                    stderr,
                    "error: <%d:%d>: incomplete byte expr '%c'\n",
                    row + 1,
                    col + 1,
                    in[i]);
                free(in);
                return -3;
            } else {
                in[nbytes++] = c2tobyte(in[i], in[i + 1]);
                ++i;
            }
            ++col;
        }
    }
    for (int i = 0; i < nbytes; putchar(in[i++]))
        ;
    free(in);
    return 0;
}
