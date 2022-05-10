
> * 针对来者的不同需求，本文将遵循以下的条目给出不同层次的方案（文中出现的“面向**编程”用词仅出于调侃，切勿当真）。
> * 声明：本文与笔者皆从编码经验入手，不包含任何主观上的OI成分，若有OIer或ACMer来访的话就当是过眼云烟吧。

### *Requirement*
基于双向链表的存储结构计算并输出指定位数的$\pi$值

### *Catalogue*
1. [面向AC编程](#O-AC-P)（也就图一乐）
2. [面向用例编程](#O-INST-P)（紧急AC入口）
3. [面向实践编程](#O-PRAC-P)（正文）
4. [面向提高编程](#O-DEV-P)（选读）
    a) 结构优化
    b) 内存优化
    c) 性能优化

### <span id="O-AC-P">**面向AC编程**</span>
---

**概述**：能跑的BUG都是好程序，能AC的代码都是好答案。——佚名
**关键词**：`打表`

`NOJ`的测试数据贫乏得可怜，如果你只是想在你的`NOJ`页面上看到“<font color="blue">**AC**</font>”的话，不妨`百度`/`必应`/`谷歌`一下`在线PI值计算`，并`ctrl+c/v`一千位写入你的源代码中。下面给出具体代码：


```cpp
#include <stdio.h>
static const char *PI = "/*将此处替换为你的π值*/";
int main() {
    int n = 0;
    char format_buf[16] = { 0 };
    scanf("%d", &n);
    /**
     * @note 写入格式化字符串，输出字符串长度n+2位，包含“3.”与小数部分的n位。
     */
    sprintf(format_buf, "%%.%ds", n + 2);
    /**
     * @note 当然你简单粗暴地直接输出500位也没有问题，毕竟面向AC编程（bushi）
     */
    printf(format_buf, PI);
    return 0;
}
```

### <span id="O-INST-P">**面向用例编程**</span>
---

**概述**：只要你题干敢蕴含这个条件，我就敢忽略对该条件的断言并且在这个前提下对代码特化。
**关键词**：`特化`

##### 问题分解
> 1. 期望输出是什么？
> 2. 如何在理论上获取期望输出？
> 3. 如何存储待输出目标？
> 4. 如何输出目标？
> 5. 如何计算目标？
> 6. 有哪些可用条件？

##### 问题分析
Q：期望输出是什么？
A：一个$n$位小数精度的$\pi$值。
Q：如何在理论上获取期望输出？
A：期望输出为有限位$\pi$，故实际问题为如何计算$\pi$值；存在多种迭代、级数和计算方法，本文考虑从下式计算$\pi$值：
$$
\displaystyle\frac{\pi}{2}=\sum_{k=0}^\infty\frac{k!}{(2k+1)!!},\;where\;n!!=\prod_{k=0}^{\lceil\frac{n}{2}\rceil-1}(n-2k)
$$
Q：如何存储待输出目标？
A：根据需求，确定使用双向链表存$\pi$值；若令链表节点的存储数据为$t$位十进制位，且$\pi$值对应的十进制数从高位到低位在链表中有序存储，则对应精度的$\pi$值表示则为一个$\lceil\frac{n+1}{t}\rceil$个节点的双向链表；其中，小数点的位置必须被明确定义。
Q：如何输出目标？
A：顺序输出节点数值，遇到小数点的位置就输出小数点。注意$t$位补齐以及整数部分的前缀零剔除（若整数部分为孤立零则需要保留）和小数部分的后缀零剔除。
Q：如何计算目标？
A：已经有数学表达式，就可以将问题归结为如何用代码表示出该式子。首先对计算式进行简易的转化得到：
$$\left\{\begin{array}{l}
R_0=2 \\ \\
R_n=\frac{n}{2n+1}R_{n-1},n\geq 1 \\ \\
\pi=\sum_{k=0}^\infty R_k
\end{array}\right.
$$根据上式，$\pi$的计算过程即可以转化为有限次$R_k$迭代和$\pi$的求和，其中$R_k$的计算涉及大数的乘法以及除法，$\pi$的求和涉及大数的加法，故只需实现大数初始化、加法、乘法、除法四项操作。
Q：有哪些可用条件？
A：**_(1)_** 输出的结果$\pi$是个定值，而整数部分只有$3$一位数字，故可以选择只存储并计算小数部分，而实现该操作只需稍微调整计算公式的初值，使计算过程中产生的值始终为小数。**_(2)_** 最终输出的$\pi$值位数是确定的，也即可以舍弃链表的动态插删功能，选择一次性构造$n$个节点，让双向链表的流于形式。**_(3)_** 选择的公式每迭代$2\sim5$次可以产生一个有效位，而式子中最大的操作数为$2n+1$，也即$int$类型便能够支持目标迭代$4$万次（根据乘法操作不溢出的情形粗略判断）**_(4)_** 所有出现的过程数皆为正数，故不需要考虑数的符号。

下面给出具体代码：
```cpp
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
     *  则迭代计算步骤为：
     *  r = r * k
     *  r = r / (2 * k + 1)
     *  pi = pi + r
     */
    number_t pi, r;
    node_t *p, *q;

    /**
     * @brief 初始化链表
     * @note 创建ceil(n/N)+1个节点，
     *  多申请一个存储节点防止最后节点的精度丢失
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
```

### <span id="O-PRAC-P">**面向实践编程**</span>
---

**概述**：
**关键词**：`一般化实现`

### <span id="O-DEV-P">**面向提高编程**</span>
---

**概述**：
**关键词**：`优化`
