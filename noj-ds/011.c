/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T011 - 循环右移（耿5.2）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 编写程序，将一维数组A(下标从1开始)中的元素循环右移k位，要求只用一个元素大小的附加存储。
 * 
 * Input
 * 第一行输入一维数组A的长度n和循环位移位数k(0<n<100;0<k<100)，用空格分开。第二行输入n个元素。
 * 
 * Output
 * 输出循环右移k位后的一维数组。
 * 
 * Sample Input
 * 6 3
 * 1 2 3 4 5 6
 * 
 * Sample Output
 * 4 5 6 1 2 3
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define N 100
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 }, *p;
	int a[N] = { 0 }, n, k, i = 0;

	scanf("%d%d", &n, &k);
	CLEAR;
	scanf("%[^\n]", buf);

	p = strtok(buf, " ");
	do {
		a[i++] = atoi(p);
		p = strtok(0L, " ");
	} while (!!p);

	for (i = 0; i < k; ++i) {
		int t = a[n - 1];
		for (int j = n - 1; j > 0; --j) {
			a[j] = a[j - 1];
		}
		a[0] = t;
	}

	for (i = 0; i < n; ++i) {
		printf("%d ", a[i]);
	}

	return 0;
}