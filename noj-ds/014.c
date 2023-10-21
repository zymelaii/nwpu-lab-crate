/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T014 - 求广义表深度（严5.30）
 *  @date: 2022/1/17
 *******************************/

/********************************
 * Description
 * 试按表头、表尾的分析方法编写求广义表的深度的递归程序。
 *
 * Input
 * 输入一串以(开始，以(结束的字符串，并且输入的左右括号必须匹配，如:(),(())...
 *
 * Output
 * 分别输出按表头、表尾分析方法求广义表深度的结果，每个结果占一行。
 *
 * Sample Input
 * ((a,b,(c,(d,e),f)),9)
 *
 * Sample Output
 * 4
 * 4
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#define ATOM 0
#define LIST 1

struct GList {
	int tag;
	union {
		char atom;
		struct GList *hp;
	};
	struct GList *tp;
};

const char* create(const char *e, struct GList *pl) {
	char ch;

	while ((ch = *e++) != '\0') {
		switch (ch) {
			case '(': {
				pl->tag = LIST;
				pl->hp = (struct GList*)malloc(sizeof(struct GList));
				pl->tp = 0L;

				memset(pl->hp, 0, sizeof(struct GList));
				pl->hp->tag = LIST;

				e = create(e, pl->hp);
				break;
			}
			case ')': {
				return e;
			}
			case ',': {
				pl->tp = (struct GList*)malloc(sizeof(struct GList));
				memset(pl->tp, 0, sizeof(struct GList));
				pl = pl->tp;
				break;
			}
			default: {
				pl->tag = ATOM;
				pl->atom = ch;
				pl->tp = 0L;
			}
		}
	}

	return e - 1;
}

void printlist(struct GList *pl) {
	while (!!pl) {
		if (pl->tag == LIST) {
			if (pl->hp || pl->tp) {
				putchar('(');
				printlist(pl->hp);
				putchar(')');
			}
		} else {
			putchar(pl->atom);
		}
		if (!!pl->tp) {
			putchar(',');
		}
		pl = pl->tp;
	}
}

int _getdepth(struct GList *pl) {
	int depth = 0;

	if (!pl) {
		return 0;
	}

	while (!!pl) {
		if (pl->tag == LIST) {
			int tmp = _getdepth(pl->hp);
			if (tmp > depth) {
				depth = tmp;
			}
		}
		pl = pl->tp;
	}

	return depth + 1;
}

int getdepth(struct GList *pl) {
	int tmp = _getdepth(pl) - 1;
	return tmp < 0 ? 0 : tmp;
}

int main(int argc, char const *argv[])
{
	char buf[1000] = { 0 };
	struct GList list;

	scanf("%[^\n]", buf);

	create(buf, &list);
	// printlist(&list);
	// putchar('\n');

	printf("%d\n%d", getdepth(&list), getdepth(&list));

	return 0;
}