/**
 * @brief SSSP problom of UG based on BFS
 * @author zymelaii
 * @date 2022-03-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <queue>

#define OO 10000

struct T {
	int from, to, weight;
	bool operator<(const T &rhs) const {
		return weight > rhs.weight;
	}
};

using Q = std::priority_queue<T>;

struct Graph {
	int *graph_, n_;
	Graph(int n) : n_(n), graph_(new int[n * n]) {}
	~Graph() { delete[] graph_; }
	int* operator[](int row) {
		return graph_ + n_ * row;
	}
};

int main(int argc, char const *argv[])
{
	int n, finished = 0;
	std::cin >> n;
	Graph G(n);
	for (int i = 0, N = n * n; i < N; ++i) {
		std::cin >> G.graph_[i];
	}

	Q pq;
	pq.push({ 0, 0, 0 });
	auto flags = new std::pair<bool,int>[n];
	memset(flags, 0, sizeof(std::pair<bool,int>)  * n);
	while (finished < n) {
		T e;
		while (!pq.empty()) {
			e = pq.top();
			pq.pop();
			if (!flags[e.to].first) {
				flags[e.to] = { true, e.weight };
				++finished;
				break;
			}
		}
		for (int i = 0; i < n; ++i) {
			if (e.to != i && G[e.to][i] != OO && !flags[i].first) {
				pq.push({ e.to, i, e.weight + G[e.to][i] });
			}
		}
	}
	for (int i = 0; i < n; ++i) {
		std::cout << flags[i].second << std::endl;
	}
	delete[] flags;

	return 0;
}