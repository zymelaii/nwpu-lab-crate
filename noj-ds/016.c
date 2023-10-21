/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T016 - 计算二叉树叶子结点数目（耿6.14）
 *  @date: 2022/1/17
 *******************************/

/********************************
 * Description
 * 二叉树按照二叉链表方式存储，编写程序，计算二叉树中叶子结点的数目。
 * 
 * Input
 * 按先序输入二叉树各结点，其中#表示取消建立子树结点。
 * 
 * Output
 * 输出二叉树中叶子节点的数目。
 * 
 * Sample Input
 * ABD##EH###CF#I##G##
 * 
 * Sample Output
 * 4
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 }, *p = buf;
	int count = 0, frame = 0;

	scanf("%s", buf);

	while (*p != '\0') {
		if (p[0] != '#') {
			++frame; // 模拟待处理节点入栈
			++p;
		} else if (p[1] == '#') {
			--frame; // 待处理节点完成，出栈
			++count; // 若后续两次操作皆为取消建立子树结点，则该结点为叶子节点
			p += 2;
		} else {
			--frame; // 待处理节点完成，出栈
			++p;
		}
	}

	printf("%d", count);

	return 0;
}