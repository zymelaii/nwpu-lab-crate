/**
 * @brief Huffman encoder/decoder
 * @author zymelaii
 * @date 2022-03-20
 */

#include <string.h>
#include <queue>
#include <string>
#include <unordered_map>
#include <iostream>

/**
 * @brief internal struct of input item and huffman tree node
 * @note ptr is designed for a easier way to implement tree construction
 */
struct T {
	char c; int weight; void *ptr;
	bool operator<(const T &rhs) const {
		return weight > rhs.weight;
	}
};

/**
 * @brief Huffman tree node with helpful constructor
 */
struct G {
	G *left, *right;
	T e;
	G(const T &t) : e(t), left(nullptr), right(nullptr) {}
	G(const T &t1, const T &t2) {
		G *g1 = t1.ptr ? static_cast<G*>(t1.ptr) : new G(t1);
		G *g2 = t2.ptr ? static_cast<G*>(t2.ptr) : new G(t2);
		e.c = 0;
		e.weight = g1->e.weight + g2->e.weight;
		e.ptr = this;
		if (g1->e.weight > g2->e.weight) {
			std::swap(g1, g2);
		}
		left = g1;
		right = g2;
	}
};

using Q = std::priority_queue<T>;
using M = std::unordered_map<char, std::string>;

/**
 * @brief Huffman Tree compounded with encode/decode functions
 */
struct HuffmanTree {
private:
	char target_;      //@ store the current on-encoding word
	std::string code_; //@ store the current decoding result
	/**
	 * @note tool function for encode procedure
	 */
	bool visit(G *node) {
		if (node == nullptr) {
			return false;
		}
		if (node->e.c == target_) {
			return true;
		}
		code_ += "0";
		if (visit(node->left)) {
			return true;
		}
		code_.back() = '1';
		if (visit(node->right)) {
			return true;
		}
		code_.erase(code_.end() - 1);
		return false;
	}
	/**
	 * @brief single word decode function
	 */
	char decode_once(const char *&s) {
		G *node = root;
		while (node->e.c == 0) {
			node = *s == '0' ? node->left : node->right;
			++s;
		}
		return node->e.c;
	}
public:
	G *root; //@ root node of huffman tree
	M map;   //@ used to cache encode results

	HuffmanTree(G *root) : root(root) {}
	/**
	 * @brief single word encode function
	 */
	const std::string& encode(char ch) {
		//@ try return cached result directly
		auto it = map.find(ch);
		if (it != map.end()) {
			return it->second;
		}
		//@ start single word encoding procedure
		target_ = ch;
		code_.clear();
		visit(root);
		map.insert({ ch, code_ });
		return map.find(ch)->second;
	}
	/**
	 * @brief encode function
	 */
	std::string encode(const char *s) {
		std::string source = "";
		while (*s) {
			source += encode(*s++);
		}
		return source;
	}
	/**
	 * @brief decode function
	 */
	std::string decode(const char *s) {
		code_.clear();
		while (*s) {
			code_ += decode_once(s);
		}
		return code_;
	}
};

/**
 * @brief wrapped input collecting procedure
 */
void input(Q &pq) {
	int n = 0;
	std::cin >> n;
	char buf[256] = { 0 }, *p;
	scanf("%[^\n0-9]", buf);
	p = strtok(buf, " ");
	while (p) {
		std::cin >> n;
		pq.push({ *p, n, nullptr });
		p = strtok(nullptr, " ");
	}
}

/**
 * @brief wrapping tree building procedure
 * @note using priority queue to select the expected terms
 * @param {pq} input (word,weight) items
 * @return root node of constructed huffman tree
 */
G* ctor(Q &pq) {
	T x = pq.top(), y;
	pq.pop();
	y = pq.top();
	pq.pop();
	G *huffman = new G(x, y);
	pq.push(huffman->e);
	while (!pq.empty()) {
		x = pq.top();
		pq.pop();
		y = pq.top();
		pq.pop();
		huffman = new G(x, y);
		if (pq.empty()) break;
		pq.push(huffman->e);
	}
	return huffman;
}

int main(int argc, char const *argv[]) {
	Q pq;
	input(pq);
	auto huffman = new HuffmanTree(ctor(pq));
	std::string source;
	std::cin >> source;
	source = huffman->encode(source.c_str());
	std::cout << source << std::endl;
	std::cout << huffman->decode(source.c_str()) << std::endl;
	return 0;
}