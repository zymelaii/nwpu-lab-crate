/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T005 - 单链表的删除（严2.29)
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 已知A,B和C为三个非递减有序的线性表，均以单链表作为存储结构。现要求对A表作如下操作:删去那些既在B表中出现又在C表中出现的元素。试对单链表编写实现上述操作的算法，并释放A表中的无用结点空间。
 * 
 * Input
 * 第一行输入3个正整数m,n,p(m,n,p<=100)，用空格分开，表三个线性表中的元素个数，其后3行依次输入A,B,C表中的元素。
 * 
 * Output
 * 输出实现上述操作后的A表。
 * 
 * Sample lnput
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

#define N 100
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

struct Node { int val, next; };

void buildlist(struct Node *buf, int n, int shift) {
	n += shift;
	for (int i = shift; i < n; ++i) {
		scanf("%d", &buf[i].val);
		if (i != 0) {
			buf[i - 1].next = i;
		}
		buf[i].next = -1;
	}
}

int intersect(struct Node *buf, int head1, int head2) {
	int p = head1, q = head2, r = -1, tail = -1, pq = -1;
	while (1) {
		while (q != -1 && buf[q].val < buf[p].val) {
			q = buf[q].next;
		}
		if (q == -1) break;
		while (p != -1 && buf[p].val < buf[q].val) {
			p = buf[p].next;
		}
		if (p == -1) break;
		if (buf[p].val == buf[q].val) {
			int np = buf[p].next;
			if (pq == -1) {
				pq = q;
				r = p;
				tail = r;
				buf[tail].next = -1;
			} else if (buf[pq].val != buf[q].val) {
				pq = q;
				buf[tail].next = p;
				tail = p;
				buf[tail].next = -1;
			}
			p = np;
			q = buf[q].next;
		}
	}
	return r;
}

void printlist(struct Node *buf, int head) {
	int p = head;
	while (p != -1) {
		printf("%d ", buf[p].val);
		p = buf[p].next;
	}
	putchar('\n');
}

int main(int argc, char const *argv[])
{
	int x, y, z;
	struct Node buf[3 * N] = { 0 };

	scanf("%d%d%d", &x, &y, &z);
	buildlist(buf, x, 0);
	buildlist(buf, y, N);
	buildlist(buf, z, N * 2);

	y = intersect(buf, N, N * 2);
	x = N * 2;
	buf[x] = (struct Node){ 0, 0 };
	while (1) {
		while (buf[x].next != -1 && buf[buf[x].next].val < buf[y].val) {
			x = buf[x].next;
		}
		if (buf[x].next == -1) break;
		while (y != -1 && buf[y].val < buf[buf[x].next].val) {
			y = buf[y].next;
		}
		if (y == -1) break;
		if (buf[buf[x].next].val == buf[y].val) {
			buf[x].next = buf[buf[x].next].next;
			x = buf[x].next;
		}
	}

	printlist(buf, buf[N * 2].next);

	return 0;
}