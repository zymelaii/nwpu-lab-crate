// gen-main <student-id> <course-id>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

const int MAIN_HEX_SIZE = 6340;
const int iPORT         = 674;

unsigned char *loadfile(FILE *fp, int *plen) {
    fseek(fp, 0, SEEK_END);
    *plen = ftell(fp);
    rewind(fp);
    unsigned char *buf = (unsigned char *)malloc(*plen);
    fread(buf, 1, *plen, fp);
    return buf;
}

int main(int argc, char *argv[]) {
    int            len;
    unsigned char *MAIN_HEX = loadfile(stdin, &len);
    if (len < MAIN_HEX_SIZE) return 1;
    printf("%d\n", (MAIN_HEX[iPORT] - 0x81) / 2 + 1);
    return 0;
}
