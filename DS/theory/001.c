/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T001 - 顺序表的插入运算（耿2.4）
 *  @date: 2022/1/11
 *******************************/

/********************************
 * Description
 * 已知顺序表L递增有序，编写程序，将X插入到线性表的适当位置上，以保持线性表的有序性。
 * 
 * Input
 * 第一行输入顺序表元素个数elenum；(0<elenum<1000)
 * 第二行输入顺序表L；
 * 第三行输入插入值X。
 * 
 * Output
 * 输出插入X后的有序顺序表
 * 
 * Sample Input
 * 7
 * 2 3 4 5 6 7 8
 * 1
 * 
 * Sample Output
 * 1 2 3 4 5 6 7 8
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define N 1000
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

int main(int argc, char const *argv[])
{
	int n, x;
	char buf[N] = { 0 }, *p;
	bool done = false;

	scanf("%d", &n);
	CLEAR;
	scanf("%[^\n]%d", buf, &x);

	p = strtok(buf, " ");
	do {
		if (!done && x <= atoi(p)) {
			printf("%d ", x);
			done = true;
		}
		printf("%s ", p);
		p = strtok(0L, " ");
	} while (!!p);
	if (!done) {
		printf("%d", x);
	}

	return 0;
}