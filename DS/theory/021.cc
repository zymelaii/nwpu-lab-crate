/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T021 - 逆波兰表达式（严7.38）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 一个四则运算算术表达式以有向无环图的邻接表方式存储，每个操作数原子都由单个字母表示。编写程序输出其逆波兰表达式。
 * 
 * Input
 * 输入四则运算算术表达式。
 * 
 * Output
 * 输出其逆波兰表达式。
 * 
 * Sample Input
 * (a+b)*c
 * 
 * Sample Output
 * ab+c*
 *******************************/

#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

typedef struct node_s {
    struct node_s *left, *right;
    char token;
    static void* operator new(size_t n) {
        static int i = 0;
        static node_s mempool[1024];
        return mempool + i++;
    }
} node_t;

inline int match_op(char c) {
    switch (c) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default: return 0;
    }
}

inline const char *move_forward(const char *p) {
    if (*p != '(') return p + 1;
    int depth = 1;
    while (depth != 0) {
        ++p;
        *p == ')' && --depth;
        *p == '(' && ++depth;
    }
    return p;
}

node_t *build(const char *begin, const char *end) {
    if (begin >= end) return nullptr;

    auto p = begin, q = p;
    int primary = INT_MAX;

    // find the last low-priority operator
    while (p < end) {
        int t = match_op(*p);
        if (t != 0 && t <= primary) {
            primary = t;
            q = p;
        }
        p = move_forward(p);
    }

    // parse atomic
    if (primary == INT_MAX) {
        if (*begin == '(') {
            return build(begin + 1, end - 1);
        } else {
            return new node_t { nullptr, nullptr, *begin };
        }
    }

    // parse operation
    return new node_t { build(begin, q), build(q + 1, end), *q };
}

void tranverse(const node_t *node) {
    if (node == nullptr) return;
    tranverse(node->left);
    tranverse(node->right);
    putchar(node->token);
}

int main(int argc, char *argv[]) {
    char buf[1024] = { 0 };
    std::cin >> buf;
    tranverse(build(buf, buf + strlen(buf)));
    return 0;
}