/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T018 - 建立二叉树的二叉链表（严6.65）
 *  @date: 2022/1/18
 *******************************/

/********************************
 * Description
 * 已知一棵二叉树的前序序列和中序序列分别存于两个一维数组中，试编写算法建立该二叉树的二叉链表。
 * 
 * Input
 * 分两行分别输入一棵二叉树的前序序列和中序序列。
 * 
 * Output
 * 输出该二叉树的后序序列。
 * 
 * Sample Input
 * ABDFGCEH
 * BFDGACEH
 * 
 * Sample Output
 * FGDBHECA
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

typedef const char* P;

void solve(P preorder, P inorder, int n) {
	if (n <= 0) return;

	char node = preorder[0]; // 先序遍历的第一个节点是根节点

	P p = inorder - 1;
	while (*++p != node); // 中序遍历的根节点划分出左右子树
	int ltlen = p - inorder;
	solve(preorder + 1, inorder, ltlen);
	solve(preorder + ltlen + 1, p + 1, n - ltlen - 1);

	putchar(node);
}

int main(int argc, char const *argv[])
{
	char preorder[1000] = { 0 }, inorder[1000] = { 0 };
	scanf("%s%s", preorder, inorder);
	solve(preorder, inorder, strlen(preorder));
	return 0;
}