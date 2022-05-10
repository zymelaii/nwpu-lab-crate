/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T002 - 线性表的就地逆置（耿2.9）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 试分别以不同的存储结构实现线性表的就地逆置算法，即在原表的存储空间将线性表(al,a2,...,an)逆置为(an,an-1,...,a1)。
 * 以一维数组作存储结构。
 * 以单链表作存储结构。
 *
 * Input
 * 第一行输入线性表元素个数elenum；（0<elenum<1000）
 * 第二行输入elenum个数，作为线性表中的元素(a1,a2,...,an)。
 *
 * Output
 * 分两行分别输出要求（1）和要求（2）的线性表逆置结果(an,an-1,...,al)。
 *
 * Sample Input
 * 5
 * 2 5 3 7 15
 *
 * Sample Output
 * 15 7 3 5 2
 * 15 7 3 5 2
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 1000
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

int main(int argc, char const *argv[])
{
	int n, x, p = -1, q, head = 0;
	int SeqList1[N] = { 0 };
	struct Node { int val, next; } SeqList2[N];

	scanf("%d", &n);
	CLEAR;

	while (scanf("%d", &x) != EOF) {
		// 链表节点插入
		SeqList2[++p] = (struct Node){ x, -1 }; // 创建新节点
		if (p - 1 >= 0) {
			SeqList2[p - 1].next = p; // 链接节点
		}
		// 数组元素追加
		SeqList1[p] = x;
	}

	p = 0;
	while (p < n - p - 1) {
		// 数组逆置
		x = SeqList1[p];
		SeqList1[p] = SeqList1[n - p - 1];
		SeqList1[n - p - 1] = x;
		++p;
	}

	p = x = head, q = SeqList2[p].next;
	SeqList2[p].next = -1;
	while (q != -1) {
		// 链表逆置
		x = q;
		q = SeqList2[q].next;
		SeqList2[x].next = p;
		p = x;
	}
	head = x;

	p = 0;
	while (p < n) {
		printf("%d ", SeqList1[p++]);
	}
	putchar('\n');

	p = head;
	while (p != -1) {
		printf("%d ", SeqList2[p].val);
		p = SeqList2[p].next;
	}
	putchar('\n');

	return 0;
}