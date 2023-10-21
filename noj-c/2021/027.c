#include <stdio.h>

int main() {
    char s[5];
    int  i, sum = 0;
    gets(s);
    for (i = 0; s[i] != '\0'; i++) {
        switch (s[i]) {
            case 'I':
                sum += 1;
                break;
            case 'V':
                sum = 5 - sum;
                break;
            case 'X':
                sum = 10 - sum;
                break;
        }
    }
    printf("%d\n", sum);
    return 0;
}
