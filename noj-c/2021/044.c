#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void add(const char *num1, const char *num2, char *result) {
    if (!result) return;
    if (!num1) {
        strcpy(result, num2);
        return;
    }
    if (!num2) {
        strcpy(result, num1);
        return;
    }
    char  mark;
    int   bits1, bits2;
    char *temp_str  = NULL;
    char *temp_str2 = NULL;
    int   temp, max, min, a;
    if (num1[0] == '-' && num2[0] != '-') {
        bits1 = strlen(num1) - 1;
        bits2 = strlen(num2);
        max   = bits1 > bits2 ? bits1 : bits2;
        min   = bits1 < bits2 ? bits1 : bits2;
        if (bits1 > bits2)
            mark = 1;
        else if (bits1 < bits2)
            mark = 0;
        else {
            temp = strcmp(num1 + 1, num2);
            if (temp > 0)
                mark = 1;
            else if (temp < 0)
                mark = 0;
            else {
                result = "0";
                return;
            }
        }
        if (mark == 1) {
            max++;
            temp_str = (char *)malloc(max + 1);
            assert(temp_str != NULL);
            memset(temp_str, '0', max + 1);
            strcpy(temp_str, num1);
            while (min-- > 0) {
                a = temp_str[--max] - num2[--bits2];
                if (a < 0) {
                    temp_str[max - 1]--;
                    a += 10;
                }
                temp_str[max] = a + '0';
            }
            temp_str2 = temp_str;
            while (*(++temp_str) == '0')
                ;
            if (*temp_str == '\0')
                result = "0";
            else {
                strcpy(result + 1, temp_str);
                result[0] = '-';
            }
            temp_str = temp_str2;
        } else {
            temp_str = (char *)malloc(max + 1);
            assert(temp_str != NULL);
            memset(temp_str, '0', max + 1);
            strcpy(temp_str, num2);
            while (min-- > 0) {
                a = temp_str[--max] - num1[bits1--];
                if (a < 0) {
                    temp_str[max - 1]--;
                    a += 10;
                }
                temp_str[max] = a + '0';
            }
            temp_str2 = temp_str;
            while (*temp_str == '0') temp_str++;
            if (*temp_str == '\0')
                result = "0";
            else
                strcpy(result, temp_str);
            temp_str = temp_str2;
        }
        if (temp_str) free(temp_str);
        return;
    } else if (num1[0] != '-' && num2[0] == '-') {
        bits1 = strlen(num1);
        bits2 = strlen(num2) - 1;
        max   = bits1 > bits2 ? bits1 : bits2;
        min   = bits1 < bits2 ? bits1 : bits2;
        if (bits1 > bits2)
            mark = 0;
        else if (bits1 < bits2)
            mark = 1;
        else {
            temp = strcmp(num1, num2 + 1);
            if (temp > 0)
                mark = 0;
            else if (temp < 0)
                mark = 1;
            else {
                result = "0";
                return;
            }
        }
        if (mark == 1) {
            max++;
            temp_str = (char *)malloc(max + 1);
            assert(temp_str != NULL);
            memset(temp_str, '0', max + 1);
            strcpy(temp_str, num2);
            while (min-- > 0) {
                a = temp_str[--max] - num1[--bits1];
                if (a < 0) {
                    temp_str[max - 1]--;
                    a += 10;
                }
                temp_str[max] = a + '0';
            }
            temp_str2 = temp_str;
            while (*(++temp_str) == '0')
                ;
            if (*temp_str == '\0')
                result = "0";
            else {
                strcpy(result + 1, temp_str);
                result[0] = '-';
            }
            temp_str = temp_str2;
        } else {
            temp_str = (char *)malloc(max + 1);
            assert(temp_str != NULL);
            memset(temp_str, '0', max + 1);
            strcpy(temp_str, num1);
            while (min-- > 0) {
                a = temp_str[--max] - num2[bits2--];
                if (a < 0) {
                    temp_str[max - 1]--;
                    a += 10;
                }
                temp_str[max] = a + '0';
            }
            temp_str2 = temp_str;
            while (*temp_str == '0') temp_str++;
            if (*temp_str == '\0')
                result = "0";
            else
                strcpy(result, temp_str);
            temp_str = temp_str2;
        }

        if (temp_str) free(temp_str);
        return;
    } else if (num1[0] == '-' && num2[0] == '-') {
        bits1     = strlen(num1) - 1;
        bits2     = strlen(num2) - 1;
        max       = bits1 > bits2 ? bits1 : bits2;
        min       = bits1 < bits2 ? bits1 : bits2;
        max      += 2;
        temp_str  = (char *)malloc(max + 1);
        assert(temp_str != NULL);
        memset(temp_str, '0', max + 1);
        if (bits1 > bits2) {
            strcpy(temp_str + 2, num1 + 1);
            while (min-- > 0) {
                a = (temp_str[--max] - '0') + (num2[bits2--] - '0');
                if (a >= 10) {
                    temp_str[max - 1]++;
                    a -= 10;
                }
                temp_str[max] = a + '0';
            }
        } else {
            strcpy(temp_str + 2, num2 + 1);
            while (min-- > 0) {
                a = (temp_str[--max] - '0') + (num1[bits1--] - '0');
                if (a >= 10) {
                    temp_str[max - 1]++;
                    a -= 10;
                }
                temp_str[max] = a + '0';
            }
        }
        temp_str2 = temp_str;
        while (*(++temp_str) == '0')
            ;
        if (*temp_str == '\0')
            result = "0";
        else {
            strcpy(result + 1, temp_str);
            result[0] = '-';
        }
        if (temp_str2) free(temp_str2);
        return;
    } else {
        bits1 = strlen(num1);
        bits2 = strlen(num2);
        max   = bits1 > bits2 ? bits1 : bits2;
        min   = bits1 < bits2 ? bits1 : bits2;
        max++;
        temp_str = (char *)malloc(max + 1);
        assert(temp_str != NULL);
        memset(temp_str, '0', max + 1);
        if (bits1 > bits2) {
            strcpy(temp_str + 1, num1);
            while (min-- > 0) {
                a = (temp_str[--max] - '0') + (num2[--bits2] - '0');
                if (a >= 10) {
                    temp_str[max - 1]++;
                    a -= 10;
                }
                temp_str[max] = a + '0';
            }
        } else {
            strcpy(temp_str + 1, num2);
            while (min-- > 0) {
                a = (temp_str[--max] - '0') + (num1[--bits1] - '0');
                if (a >= 10) {
                    temp_str[max - 1]++;
                    a -= 10;
                }
                temp_str[max] = a + '0';
            }
        }
        temp_str2 = temp_str;
        while (*temp_str == '0') temp_str++;
        if (*temp_str == '\0')
            result = "0";
        else
            strcpy(result, temp_str);
        if (temp_str2) free(temp_str2);
        return;
    }
}

int main() {
    char a[100], b[100], c[100];
    gets(b);
    gets(c);
    add(b, c, a);
    printf("%s", a);
    return (0);
}
