#include <stdio.h>
#include <malloc.h>
#include <stddef.h>

#define N 4     //! 每个节点存储四位十进制
#define R 10000 //! 模数10^4

typedef unsigned int int_t;

typedef struct node_s {
	int_t value;
	struct node_s *prev, *next;
} node_t;

typedef struct number_s {
	node_t *head, *tail;
} number_t;

int main() {
	int n = 0, t, k;
	scanf("%d", &n);

	/**
	 * @note 令pi的初值为R0+R1+R2，r的初值为R2，
	 * 	则迭代计算步骤为：
	 * 	r = r * k
	 * 	r = r / (2 * k + 1)
	 *  pi = pi + r
	 */
	number_t pi, r;
	node_t *p, *q;

	/**
	 * @brief 初始化链表
	 * @note 创建ceil(n/N)+1个节点，
	 * 	多申请一个存储节点防止最后节点的精度丢失
	 */
	t = (n + N - 3) / N + 1;
	p = pi.head = (node_t*)malloc(sizeof(node_t));
	q = r.head  = (node_t*)malloc(sizeof(node_t));
	p->value = 9333;
	q->value = 2666;
	p->prev = NULL;
	q->prev = NULL;
	while (--t > 0) {
		p->next = (node_t*)malloc(sizeof(node_t));
		q->next = (node_t*)malloc(sizeof(node_t));
		p->next->prev = p;
		q->next->prev = q;
		p = p->next;
		q = q->next;
		p->value = 3333;
		q->value = 6666;
	}
	p->next = q->next = NULL;
	pi.tail = p;
	r.tail  = q;

	/**
	 * @brief π值计算
	 * @note 固定迭代2500次
	 */
	t = 2;
	while (++t < 2500) {
		int_t carrier   = 0; //! 进位数
		int_t remainder = 0; //! 余数

		//! r = r * k
		k = t;
		q = r.tail;
		while (q) {
			q->value = carrier + q->value * k;
			carrier = q->value / R;
			q->value %= R;
			q = q->prev;
		}

		//! r = r / (2 * k + 1)
		k = 2 * t + 1;
		q = r.head;
		while (q) {
			q->value += remainder * R;
			remainder = q->value % k;
			q->value /= k;
			q = q->next;
		}

		//! pi = pi + r
		carrier = 0;
		p = pi.tail;
		q = r.tail;
		while (p) {
			p->value += q->value + carrier;
			carrier = p->value / R;
			p->value %= R;
			p = p->prev;
			q = q->prev;
		}
	}

	/**
	 * @brief 输出π值
	 */
	putchar('3');
	putchar('.');
	p = pi.head;
	t = (n - 1) / N;
	while (t-- > 0) {
		printf("%04d", p->value);
		p = p->next;
	}
	t = N - (n - 1) % N - 1;
	while (t-- > 0) {
		p->value /= 10;
	}
	printf("%d", p->value);

	/**
	 * @brief 释放内存
	 */
	p = pi.head;
	q = r.head;
	while (p->next) {
		p = p->next;
		q = q->next;
		free(p->prev);
		free(q->prev);
	}
	free(p);
	free(q);

	return 0;
}
