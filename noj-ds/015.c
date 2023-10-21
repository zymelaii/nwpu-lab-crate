/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T015 - 建立二叉树的二叉链表存储结构（严6.70）
 *  @date: 2022/1/17
 *******************************/

/********************************
 * Description
 * 如果用大写字母标识二叉树结点，则一颗二叉树可以用符合下面语法图的字符序列表示。试编写递归程序，由这种式的字符序列，建立相应的二叉树的二叉链表存储结构（附图见《严蔚敏:数据结构题集（C语言版)》第45页6.70）。
 * 
 * Input
 * 输入如图所示的字符序列。
 * 
 * Output
 * 建立相应二叉树的二成叉链表存储结构，并先序遍历输出。
 * 
 * Sample Input
 * A(B(#,D),C(E(#,F),#))
 * 
 * Sample Output
 * AB#DCE#F#
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

struct Node {
	char x;
	struct Node *left, *right;
};

void _build(const char *l, const char *r, struct Node *node) {
	if (l >= r) {
		node->left = node->right = 0L;
		return;
	}

	int depth = 0;
	const char *p = l;

	while (++p <= r) {
		if (*p == '(') {
			++depth;
		} else if (*p == ')') {
			--depth;
		}
		if (depth == 0 && *p == ',') {
			break;
		}
	}

	node->left = (struct Node*)malloc(sizeof(struct Node));
	node->left->x = l[0];
	_build(l + 2, p - 2, node->left);

	node->right = (struct Node*)malloc(sizeof(struct Node));
	node->right->x = p[1];
	_build(p + 3, r - 2, node->right);
}

struct Node* build(const char *input) {
	const int len = strlen(input);
	if (len <= 0) {
		return 0L;
	}

	struct Node *root = (struct Node*)malloc(sizeof(struct Node));
	root->x = input[0];
	_build(input + 2, input + len - 1, root);

	return root;
}

void preorder(struct Node *node) {
	if (node == 0L) {
		return;
	} else {
		putchar(node->x);
	}

	preorder(node->left);
	preorder(node->right);
}

void destroy(struct Node *node) {
	if (!node) {
		return;
	}

	destroy(node->left);
	destroy(node->right);

	free(node);
}

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 };

	scanf("%s", buf);

	struct Node *tree = build(buf);
	preorder(tree);

	destroy(tree);
	return 0;
}