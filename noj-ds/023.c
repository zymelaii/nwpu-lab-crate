/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T023 - 构造哈希表（耿8.12）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 选取哈希函数H(k)=(3k)%11，用线性探测再散列法处理冲突。试在0-10的散列地址空间中，编写程序，对关键字序列(22,41,5346,30,13,01,67)构造哈希表，并求等概率情况下查找成功的平均查找度。
 * 
 * Input
 * 无
 * 
 * Output
 * 输出等概率情况下查找成功的平均查找长度。
 * 
 * Sample Input
 * 
 * Sample Output
 * 2
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>

typedef struct {
    bool occupied;
    int value;
} unit_t;

int hash(int key) {
    return (3 * key) % 11;
}

static const int in[] = { 22, 41, 5346, 30, 13, 01, 67 };
#define N (sizeof(in) / sizeof(int))

void build(unit_t *map) {
    for (int i = 0; i < N; ++i) {
        int index = hash(in[i]);
        while (map[index].occupied) {
            ++index;
        }
        map[index].value = in[i];
        map[index].occupied = true;
    }
}

int measure_lookup(unit_t *map, int key) {
    int index = hash(key);
    int value = map[index].value, count = 1;
    while (index < 11 && value != key) {
        value = map[++index].value;
        ++count;
    }
    return count;
}

int main() {
    unit_t map[11] = { 0 };
    build(map);
    int count = 0;
    for (int i = 0; i < N; ++i) {
        count += measure_lookup(map, in[i]);
    }
    printf("%d", (int)(count * 1.0 / N + 0.5));
    return 0;
}