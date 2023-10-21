#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void jiami(char s[100]) {
    int i;
    for (i = 0; i < strlen(s); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] = s[i] + 3;
        if (s[i] >= 'a' && s[i] <= 'z') s[i] = s[i] - 3;
    }
}

int main() {
    char s[100];
    gets(s);
    jiami(s);
    puts(s);
    return 0;
}
