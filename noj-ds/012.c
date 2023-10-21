/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T012 - 以三元组表为存储结构实现矩阵相加（耿5.7）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 假设稀疏矩阵A和B均以三元组表作为存储结构。试编写矩阵相的程序，另设三元组表C存放结果矩阵。矩阵大小为m行n列(0<m,n<100)
 * 
 * Input
 * 第一行输入tl，t2(0<t1,t2<100) ，t1和t2分别是矩阵A和B中非零元素的个数，后面t1+t2行分别输入A和B中的元素，用三元组表示。
 * 
 * Output
 * 输出三元组表C。
 * 
 * Sample Input
 * 3 3
 * 1 2 3
 * 3 2 1
 * 3 4 2
 * 1 1 4
 * 3 2 5
 * 3 4 1
 * 
 * Sample Output
 * 1 1 4
 * 1 2 3
 * 3 2 6
 * 3 4 3
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct T { int i, j, x; };

int main(int argc, char const *argv[])
{
	struct T A[100] = { 0 }, B[100] = { 0 }, C[100] = { 0 };
	int u = 0, v = 0, t1, t2, t = 0;

	scanf("%d%d", &t1, &t2);
	for (int i = 0; i < t1; scanf("%d%d%d", &A[i].i, &A[i].j, &A[i].x), ++i);
	for (int i = 0; i < t2; scanf("%d%d%d", &B[i].i, &B[i].j, &B[i].x), ++i);

	while (u < t1 && v < t2) {
		if (A[u].i < B[v].i) {
			C[t++] = A[u++];
		} else if (A[u].i > B[v].i) {
			C[t++] = B[v++];
		} else {
			if (A[u].j < B[v].j) {
				C[t++] = A[u++];
			} else if (A[u].j > B[v].j) {
				C[t++] = B[v++];
			} else {
				C[t] = (struct T){ A[u].i, A[u].j, A[u].x + B[v].x };
				if (C[t].x != 0) {
					++t;
				}
				++u, ++v;
			}
		}
	}

	while (u < t1) {
		C[t++] = A[u++];
	}

	while (v < t2) {
		C[t++] = B[v++];
	}

	for (int i = 0; i < t; ++i) {
		printf("%d %d %d\n", C[i].i, C[i].j, C[i].x);
	}

	return 0;
}