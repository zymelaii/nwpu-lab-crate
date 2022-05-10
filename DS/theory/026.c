/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T026 - 二叉排序树的合并（严9.38）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 试编写程序，将两棵二叉排序树合并为一棵二叉排序树。
 * 
 * Input
 * 按照先序序列，分两行输入两棵二叉排序树各结点（结点值大于0)，其中-1表示取消建立子树结点。
 * 
 * Output
 * 按照中序序列输出合并后的二叉排序树
 * 
 * Sample Input
 * 12 8 4 -1 -1 10 -1 -1 16 13 -1 -1 18 -1 -1
 * 17 6 2 -1 -1 9 -1 -1 24 19 -1 -1 26 -1 -1
 * 
 * Sample Output
 * 2 4 6 8 9 10 12 13 16 17 18 19 24 26
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>

typedef struct node_s {
    struct node_s *left, *right;
    int value;
    bool lthread, rthread;
} node_t;

void build(node_t *node) {
    int value;
    scanf("%d", &value);
    if (value == -1) {
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
    node->lthread = node->left == NULL;
    node->rthread = node->right == NULL;
}

void lin_threadify(node_t *prev, node_t *node) {
    if (node == NULL) return;
    if (node->lthread) {
        node->left = prev;
        if (node->rthread) return;
    }
    lin_threadify(prev, node->lthread ? NULL : node->left);
    lin_threadify(node, node->rthread ? NULL : node->right);
}

void rin_threadify(node_t *prev, node_t *node) {
    if (node == NULL) return;
    if (node->rthread) {
        node->right = prev;
        if (node->lthread) return;
    }
    rin_threadify(node, node->lthread ? NULL : node->left);
    rin_threadify(prev, node->rthread ? NULL : node->right);
}

node_t* begin(node_t *tree) {
    if (tree == NULL) return NULL;
    while (tree->left) {
        tree = tree->left;
    }
    return tree;
}

node_t* rbegin(node_t *tree) {
    if (tree == NULL) return NULL;
    while (tree->right) {
        tree = tree->right;
    }
    return tree;
}

node_t* next(node_t *node) {
    bool is_rthread = node->rthread;
    node = node->right;
    if (is_rthread) return node;
    while (!node->lthread) {
        node = node->left;
    }
    return node;
}

node_t* prev(node_t *node) {
    bool is_lthread = node->lthread;
    node = node->left;
    if (is_lthread) return node;
    while (!node->rthread) {
        node = node->right;
    }
    return node;
}

void insert(node_t *tree, int value) {
    node_t *it = begin(tree);
    while (it != NULL && it->value <= value) {
        it = next(it);
    }

    node_t *node = (node_t*)malloc(sizeof(node_t));
    node->value = value;

    if (it == NULL) {
        it = rbegin(tree);
        it->right = node;
        it->rthread = false;
        node->left = NULL;
        node->right = NULL;
    } else {
        node->left = it->lthread ? NULL : it->left;
        node->right = NULL;
        it->left = node;
        it->lthread = false;
    }

    node->lthread = node->left == NULL;
    node->rthread = node->right == NULL;

    lin_threadify(NULL, tree);
    rin_threadify(NULL, tree);
}

void inorder(node_t *node) {
    if (node == NULL) return;
    inorder(node->lthread ? NULL : node->left);
    printf("%d ", node->value);
    inorder(node->rthread ? NULL : node->right);
}

void merge(node_t *tree, node_t *node) {
    if (node == NULL) return;
    merge(tree, node->lthread ? NULL : node->left);
    insert(tree, node->value);
    merge(tree, node->rthread ? NULL : node->right);
}

int main(int argc, char const *argv[])
{
    node_t tree[2];

    build(tree);
    lin_threadify(NULL, tree);
    rin_threadify(NULL, tree);

    build(tree + 1);
    lin_threadify(NULL, tree + 1);
    rin_threadify(NULL, tree + 1);

    merge(tree, tree + 1);

    inorder(tree);

    return 0;
}