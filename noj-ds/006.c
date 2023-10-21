/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T006 - LOCATE操作（严2.38）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 设有一个双向循环链表，每个结点中除有pre,data和next三个域外，还增设了一个访问频度域freq。在链表被起用之前，频度域freq的值均初始化为零，而每当对链表进行一次LOCATE (Lx)的操作后，被访问的结点(即元素值等于x的结点)中的频度域freq的值便增1，同时调整链表中结点之间的次序，使其按访问频度非递增的次序排列，以便始终保持被频繁访问的结点总是靠近表头结点。试编写符合上述要求的LOCATE操作的程序。
 * 
 * Input
 * 第一行输入双向循环链表的节点数m和被访问的节点数n，
 * 第二行输入双向循环链表各节点的值，
 * 第三行依次输入被访问的节点。
 * 
 * Output
 * 输出符合上述要求的双向循环链表。输出经过n次LOCATE后的链表。
 * 
 * Sample Input
 * 7 1
 * a b c d e f g
 * d
 * 
 * Sample Output
 * d a b c e f g
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

struct Node {
	int   freq;
	char *data;
	struct Node *prev;
	struct Node *next;
};

void printlist(struct Node *head) {
	struct Node *p = head;
	do {
		printf("%s ", p->data);
		p = p->next;
	} while (p != head);
	putchar('\n');
}

int main(int argc, char const *argv[])
{
	int i = 0, j = 0;
	char strbuf[1000] = { 0 }, buf[1000] = { 0 }, *p;
	struct Node nodebuf[1000] = { 0 }, *q = nodebuf, *head = nodebuf;
	scanf("%2*d");
	CLEAR;
	scanf("%[^\n]", strbuf);
	CLEAR;
	scanf("%[^\n]", buf);

	p = strtok(strbuf, " ");
	do {
		q->freq = 0;
		q->data = p;
		q->prev = q - 1;
		q->next = q + 1;
		q = q->next;
		p = strtok(0L, " ");
	} while (!!p);
	--q;
	q->next = nodebuf;
	q->next->prev = q;

	p = strtok(buf, " ");
	do {
		q = head;
		if (q != q->next) {
			while (strcmp(q->data, p) != 0) {
				q = q->next;
			}
			++q->freq;
			if (q != head) {
				q->prev->next = q->next;
				q->next->prev = q->prev;
				struct Node *cursor = head;
				while (q->freq <= cursor->freq) {
					cursor = cursor->next;
				}
				cursor = cursor->prev;
				q->next = cursor->next;
				q->prev = cursor;
				cursor->next->prev = q;
				cursor->next = q;
				if (q->freq > head->freq) {
					head = q;
				}
			}
		}
		p = strtok(0L, " ");
	} while (!!p);

	printlist(head);

	return 0;
}