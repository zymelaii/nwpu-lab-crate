/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T017 - 输出以二叉树表示的算术表达式（严6.51）
 *  @date: 2022/1/17
 *******************************/

/********************************
 * Description
 * 编写程序，输出以二叉树表示的算术表达式，若该表达式中含有括号，则在输出时应添上。
 * 
 * Input
 * 按先序输入一行字符，其中#表示取消建立子树结点，即所有叶子节点均为#。
 * 
 * Output
 * 输出该二叉树所表示的算术表达式（若表达式中含有括号，则在输出时应添上）。
 * 
 * Sample Input
 * *+a(###b#)##c##
 * 
 * Sample Output
 * (a+b)*c
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

void build(struct Node *node) {
	node->x = getchar();
	if (node->x == EOF || node->x == '#') {
		node->x = '#';
		node->left = node->right = 0L;
		return;
	}

	node->left = (struct Node*)malloc(sizeof(struct Node));
	node->right = (struct Node*)malloc(sizeof(struct Node));
	build(node->left);
	build(node->right);
}

void inorder(struct Node *node) {
	if (node == 0L || node->x == '#') {
		return;
	}

	inorder(node->left);
	putchar(node->x);
	inorder(node->right);
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
	struct Node tree = { 'x', 0L, 0L };
	build(&tree);
	inorder(&tree);
	destroy(tree.left);
	destroy(tree.right);
	return 0;
}