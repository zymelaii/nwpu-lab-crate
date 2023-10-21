/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T009 - 循环队列（严3.30）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 假设将循环队列定义为:以域变量rear和length分别指示循环队列中队尾元素的位置和内含元素的个数。编写相应的入队列和出队列的程序，并判断循环队列是否队满(在出队列的算法中要返回队头元素)。
 * 
 * Input
 * 第一行输入队列大小N,第二行开始输入若干入队元素，队满时，停止入队，队满时输出yes，否则输出no。第三行输入出队元素。
 * 
 * Output
 * 输出出队操作后的循环队列，并返回出队后的队头元素。
 * 
 * Sample Input
 * 5
 * 3 4 6 2 7
 * yes
 * 4
 * 
 * Sample Output
 * 6 2 7
 * 6
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#define CLEAR { int c; while ((c = getchar()) != EOF && c != '\n'); }

static int MAX_QLEN;

struct Q { int *queue, rear, length; };

bool push(struct Q *queue, int e) {
	if (queue->length == MAX_QLEN) {
		return false;
	}
	++queue->length;
	queue->queue[queue->rear] = e;
	queue->rear = (queue->rear + 1) % MAX_QLEN;
	return true;
}

bool pop(struct Q *queue, int *e) {
	if (queue->length == 0) {
		return false;
	}
	int front = (queue->rear - queue->length-- + MAX_QLEN) % MAX_QLEN;
	if (!!e) {
		*e = queue->queue[front];
	}
	return true;
}

void printqueue(struct Q *queue) {
	for (int i = queue->length; i > 0; --i) {
		int front = (queue->rear - i + MAX_QLEN) % MAX_QLEN;
		printf("%d ", queue->queue[front]);
	}
	putchar('\n');
}

int main(int argc, char const *argv[])
{
	int t, x;
	struct Q queue;
	char buf[1000] = { 0 }, *p = buf;

	scanf("%d", &MAX_QLEN);
	CLEAR;
	scanf("%[^\n]", buf);
	CLEAR;
	scanf("%*s"); // 帅逼东西，题目也不讲清楚……
	scanf("%d", &t);

	queue.queue = (int*)malloc(sizeof(int) * MAX_QLEN);
	queue.rear = queue.length = 0;

	p = strtok(buf, " ");
	do {
		if (!push(&queue, atoi(p))) break;
		p = strtok(0L, " ");
	} while (!!p);

	// 啥玩意儿！骗我TE！
	// puts(queue.length == MAX_QLEN ? "yes" : "no");

	while ((pop(&queue, &x), x) != t);
	printqueue(&queue);
	printf("%d", queue.queue[(queue.rear - queue.length + MAX_QLEN) % MAX_QLEN]);

	free(queue.queue);
	return 0;
}