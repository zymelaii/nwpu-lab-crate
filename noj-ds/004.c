/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T004 - 单链表的归并（耿2.11)
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 假设两个按元素值非递减有序排列的线性表A和B,均以单链表作为存储结构，试编写程序，将A表和B表归并成一个按元素值非递增有序排列的线性表C，并要求利用原表(即A表和B表的)结点空间存放表C。
 * 
 * lnput
 * 第一行输入两个正整数m,n(m,n<=100),用空格分开，分别表示线性表A和中元素个数，其后两行分别输入单链表A和B。
 * 
 * Output
 * 输出单链表C。
 * 
 * Sample lnput
 * 5 5
 * 1 3 7 12 16
 * 2 6 7 13 20
 * 
 * Sample Output
 * 20 16 13 12 7 7 6 3 2 1
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 200
#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

struct Node { int val, next; };

int reverse(struct Node *buf, int head) {
	int p = head, q = buf[p].next, r = p;
	buf[p].next = -1;
	while (q != -1) {
		r = q;
		q = buf[q].next;
		buf[r].next = p;
		p = r;
	}
	head = r;
	return head;
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
	int m, n, p, q, r = N;
	struct Node buf[N + 1];

	scanf("%d%d", &m, &n);
	for (int i = 0; i < m + n; ++i) {
		scanf("%d", &buf[i].val);
		buf[i].next = -1;
		if (i != 0 && i != m) {
			buf[i - 1].next = i;
		}
	}

	q = n = reverse(buf, m);
	p = m = reverse(buf, 0);

	while (p != -1 && q != -1) {
		if (buf[p].val > buf[q].val) {
			buf[r].next = p;
			r = p;
			p = buf[p].next;
		} else {
			buf[r].next = q;
			r = q;
			q = buf[q].next;
		}
		buf[r].next = -1;
	}

	if (p != -1) {
		buf[r].next = p;
	} else {
		buf[r].next = q;
	}
 
	printlist(buf, buf[N].next);

	return 0;
}