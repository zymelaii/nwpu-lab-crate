/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T008 - 逆波兰式（耿3.8）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 假设表达式由单字母变量和双目四则运算算符构成。试编写程序，将一个通常书写形式且书写正确的表达式转换为逆波兰式。
 * 
 * Input
 * 输入由单字母变量和双目四则运算算符构成的表达式。
 * 
 * Output
 * 输出其逆波兰式。
 * 
 * Sample Input
 * (a+b)*c
 * 
 * Sample Output
 * ab+c*
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PBrace 3
struct E { char c; short priority; };

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 }, rpn[1000] = { 0 }, *p = buf - 1, *q = rpn;
	int i = -1;
	struct E STACK[1000];

	scanf("%s", buf);

	while (*++p != '\0') {
		switch (*p) {
			case '+': case '-': while (i >= 0 && STACK[i--].priority <= 2) {
				*q++ = STACK[i + 1].c;
			}; STACK[++i] = (struct E){ *p, 2 }; break;
			case '*': case '/': while (i >= 0 && STACK[i--].priority <= 1) {
				*q++ = STACK[i + 1].c;
			}; STACK[++i] = (struct E){ *p, 1 };  break;
			case '(': STACK[++i] = (struct E){ '(', PBrace }; break;
			case ')': while (i >= 0 && STACK[i--].priority != PBrace) {
				*q++ = STACK[i + 1].c;
			}; break;
			default: *q++ = *p;
		}
	}

	while (i-- >= 0) {
		if (STACK[i + 1].priority != 0) {
			*q++ = STACK[i + 1].c;
		}
	}

	puts(rpn);

	return 0;
}