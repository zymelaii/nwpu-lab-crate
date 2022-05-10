/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T010 - k阶斐波那契数列（严3.32）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 试利用循环队列编写k阶斐波那契数列中前n+1项（f(0),f(1)...,f(n)）的程序，要求满足: f(n)<=max而f(n+1)>max,其中max为某个约定的常数。(注意:本题所用循环队列的容量仅为k，则在程序执行结束时，留在循环队列中的元素应是所求k阶斐波那契序列中的最后k项f(n-k+1),. .,f(n)).
 * 
 * Input
 * 输入常数max (O<max<10000)，阶数k (1<k<100)，用空格隔开。
 * 
 * Output
 * 输出k阶斐波那契数列中的最后k项f(n-k+1),...,f(n)。
 * 
 * Sample Input
 * 14 2
 * 
 * Sample Output
 * 8 13
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

static int MAX_QLEN;

struct Q { unsigned long long *queue; int rear, length; };

unsigned long long front(struct Q *queue) {
	return queue->queue[(queue->rear - queue->length + MAX_QLEN) % MAX_QLEN];
}

unsigned long long rear(struct Q *queue) {
	return queue->queue[(queue->rear - 1 + MAX_QLEN) % MAX_QLEN];
}

unsigned long long at(struct Q *queue, int index) {
	int pos = queue->rear + index + MAX_QLEN;
	if (index >= 0) {
		return queue->queue[(pos - queue->length) % MAX_QLEN];
	} else {
		return queue->queue[(queue->rear + index + MAX_QLEN) % MAX_QLEN];
	}
}

bool push(struct Q *queue, unsigned long long e) {
	if (queue->length == MAX_QLEN) {
		return false;
	}
	++queue->length;
	queue->queue[queue->rear] = e;
	queue->rear = (queue->rear + 1) % MAX_QLEN;
	return true;
}

bool pop(struct Q *queue, unsigned long long int *e) {
	if (queue->length == 0) {
		return false;
	}
	if (!!e) {
		*e = front(queue);
	}
	--queue->length;
	return true;
}

void printqueue(struct Q *queue) {
	for (int i = 0; i < queue->length; ++i) {
		printf("%llu ", at(queue, i));
	}
	putchar('\n');
}

int main(int argc, char const *argv[])
{
	unsigned long long max, u = 0, v;
	struct Q queue;

	scanf("%d%d", &max, &MAX_QLEN);

	queue.queue = (unsigned long long*)malloc(sizeof(unsigned long long) * MAX_QLEN);
	queue.rear = queue.length = 0;

	push(&queue, 1);
	push(&queue, 1);
	for (int i = 2; i < MAX_QLEN; ++i) {
		push(&queue, rear(&queue) + at(&queue, -2));
	}

	for (int i = 0; i < MAX_QLEN - 1; ++i) {
		u += queue.queue[i];
	}

	while (1) {
		v = rear(&queue);
		if (v <= max && u + v > max) break;
		u += v;
		pop(&queue, &v);
		push(&queue, u);
		u -= v;
	}

	printqueue(&queue);

	free(queue.queue);
	return 0;
}