#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *fp = fopen("DATA5609.DAT", "rb");
    if (fp == NULL) return -1;
    char buf[11] = "";
    fread(buf, sizeof(buf), 1, fp);
    if (strncmp("JFIF", buf + 6, strlen("JFIF")) == 0) {
        printf("JPEG");
    } else if (strncmp("GIF89a", buf, strlen("GIF89a")) == 0) {
        printf("GIF");
    } else if (strncmp("PNG", buf + 1, strlen("PNG")) == 0) {
        printf("PNG");
    } else {
        printf("UNKNOW");
    }
    return 0;
}
