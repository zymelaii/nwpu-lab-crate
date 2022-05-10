/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T024 - 二叉排序树的判别（耿8.6）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 试编写程序，判别给定的二叉树是否为二叉排序树。设此二叉树以二叉链表作存储结构，且树中结点的关键字均不同。
 * 
 * Input
 * 按先序输入二叉树各结点（结点值大于0），其中-1表示取消建立子树结点。
 * 
 * Output
 * 若该二叉树为二叉排序树，则输出yes；否则，输出no。
 * 
 * Sample Input
 * 12 8 4 -1 -1 10 -1 -1 16 13 -1 -1 18 -1 -1
 * 
 * Sample Output
 * yes
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct node_s {
    struct node_s *left, *right;
    int value;
} node_t;

void build(node_t *node) {
    int value;
    scanf("%d", &value);
    if (value == -1) {
        node->left = NULL;
        node->right = NULL;
        node->value = -1;
        return;
    }
    node->value = value;
    node->left = (node_t*)malloc(sizeof(node_t));
    node->right = (node_t*)malloc(sizeof(node_t));
    build(node->left);
    build(node->right);
    if (node->left->value == -1) {
        free(node->left);
        node->left = NULL;
    }
    if (node->right->value == -1) {
        free(node->right);
        node->right = NULL;
    }
}

bool check(node_t *node) {
    if (node == NULL) return true;
    bool result = true;
    if (node->left != NULL) {
        result = result && check(node->left)
            && node->left->value <= node->value;
    }
    if (node->right != NULL) {
        result = result && check(node->right)
            && node->right->value >= node->value;
    }
    return result;
}

int main(int argc, char const *argv[])
{
    node_t root;
    build(&root);
    puts(check(&root) ? "yes" : "no");
    return 0;
}