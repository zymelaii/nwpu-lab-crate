/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T007 - 表达式括号匹配（严3.19）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 假设一个算术表达式中可以包含三种括号:圆括号“（”和“)”、方括号“["和“] ”和花括号"{"和“}”，且这三种括号可按任意的次序嵌套使用(如:..[....}..[.}..}...]..(...)..)。编写判别给定表达式中所含括号是否正确对出现的程序(已知表达式已存入数据元素为字符的顺序表中)。
 * 
 * Input
 * 输入算术表达式，换行结束。
 * 
 * Output
 * 若给定表达式中所含括号正确配对，则输出yes，否则输出no。
 * 
 * Sample Input
 * [5+(6-3)]-(2+3)]
 * 
 * Sample Output
 * no
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct E { short type, depth; };
enum { S = 1, M, B };

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 }, *p = buf - 1;
	struct E STACK[1000] = { 0 };
	int q = -1;
	bool pass = true;

	scanf("%s", buf);

#define PUSH(T) do {                     \
	if (q < 0 || STACK[q].type != T) {   \
		STACK[++q] = (struct E){ T, 1 }; \
	} else { ++STACK[q].depth; }         \
} while (0);

#define POP(T) do {                      \
	if (q < 0 || STACK[q].type != T) {   \
		pass = false;                    \
	} else if (--STACK[q].depth == 0) {  \
		--q;                             \
	}                                    \
} while (0);

	while (*++p != '\0' && pass) {
		switch (*p) {
			case '(': PUSH(S); break;
			case '[': PUSH(M); break;
			case '{': PUSH(B); break;
			case ')': POP(S);  break;
			case ']': POP(M);  break;
			case '}': POP(B);  break;
		}
	}

	puts(pass ? "yes" : "no");

	return 0;
}