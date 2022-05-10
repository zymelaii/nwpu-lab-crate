/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T003 - 顺序表的删除（严2.29)
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 已知A,B和C为三个非递减有序的线性表，现要求对A表作如下操作:删去那些既在B表中出现又在C表中出现的元素。试对顺序表编写实现上述操作的算法。
 * 
 * Input
 * 第一行输入3个正整数m,n,p(m,n,p<=100)，用空格分开，分别表示三个线性表中的元素个数，其后3行依次输入A,B,C表中的元素。
 * 
 * Output
 * 输出实现上述操作后的A表。
 * 
 * Sample Input
 * 8 5 6
 * 1 2 3 4 5 6 6 7
 * 2 3 5 9 12
 * 2 4 5 6 12 13
 * 
 * Sample Output
 * 1 3 4 6 6 7
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 101
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

int main(int argc, char const *argv[])
{
	int x, y, z;
	int buf[3][N], p, q, r;

	scanf("%d%d%d", &x, &y, &z);
	for (int i = 0; i < x; scanf("%d", buf[0] + i++));
	for (int i = 0; i < y; scanf("%d", buf[1] + i++));
	for (int i = 0; i < z; scanf("%d", buf[2] + i++));

	p = q = r = 0;
	while (1) {
		while (q < y && buf[1][q] < buf[2][r]) {
			++q;
		}
		if (q == y) break;
		while (r < z && buf[2][r] < buf[1][q]) {
			++r;
		}
		if (r == z) break;
		if (buf[1][q] == buf[2][r]) {
			if (p == 0 || p > 0 && buf[1][p - 1] != buf[1][q]) {
				buf[1][p++] = buf[1][q];
			}
			++q, ++r;
		}
	}

	y = p, p = q = 0;
	while (p < x) {
		while (q < y && buf[1][q] < buf[0][p]) {
			++q;
		}
		if (!(q < y && buf[0][p] == buf[1][q])) {
			printf("%d ", buf[0][p]);
		}
		++p;
	}

	return 0;
}