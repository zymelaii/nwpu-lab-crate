#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int h;

void fun(char s[], char *p) {
    int i, j = 0, t;
    for (i = 1; i < strlen(s); i = i + 2) {
        t = s[i];
        if (t % 2 != 0) {
            p[j] = s[i];
            j++;
        }
    }
    h = j;
}

int main() {
    char s[100], p[100], *a;
    int  i;
    a = p;
    gets(s);
    fun(s, a);
    for (i = 0; i < h; i++) printf("%c", p[i]);
    return 0;
}
