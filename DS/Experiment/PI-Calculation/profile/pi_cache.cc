#pragma once
#include "pi_template.cc"

template <typename IntType, size_t NumPerUnit,
	local_traits::Require<IntType, NumPerUnit> = 0>
struct Freespace_Cache {
	static constexpr size_t  N = NumPerUnit;
	static constexpr IntType R = local_traits::pow10_t<NumPerUnit>::value;

	struct Node {
		Node *prev_, *next_;
		IntType x_[4];
		Node() {}
		Node(IntType x1, IntType x2, IntType x3, IntType x4, Node *prev, Node *next)
			: prev_(prev), next_(next)
		{
			x_[0] = x1;
			x_[1] = x2;
			x_[2] = x3;
			x_[3] = x4;
			if (prev) {
				prev->next_ = this;
			}
			if (next) {
				next->prev_ = this;
			}
		}
	};

	const size_t max_precision_;
	size_t precision_;
	Node *head_, *tail_;
	Node *base_;

	Freespace_Cache() = delete;

	Freespace_Cache(const Freespace_Cache &num)
		: max_precision_(num.max_precision_), precision_(num.precision_)
	{
		Node *node = num.head_;
		head_ = allocate(node->x_, nullptr, nullptr);
		base_ = head_;

		while (node != num.base_) {
			node = node->next_;
			base_ = allocate(node->x_, base_, nullptr);
		}

		tail_ = base_;
		node = node->next_;

		while (node) {
			tail_ = allocate(node->x_, tail_, nullptr);
			node = node->next_;
		}
	}

	Freespace_Cache(size_t precision, const IntType value)
		: max_precision_((precision + Freespace_Cache::N - 1) / Freespace_Cache::N), precision_(0)
	{
		const IntType ints[4] = { value, 0, 0, 0 };
		base_ = allocate(ints, nullptr, nullptr);
		head_ = base_;
		tail_ = base_;
	}

	~Freespace_Cache()
	{
		head_->next_ = freespace;
		freespace = head_;

		while (freespace) {
			auto next = freespace->next_;
			delete freespace;
			freespace = next;
		}

		head_ = nullptr;
		base_ = nullptr;
		tail_ = nullptr;
		precision_ = 0;
	}

	Freespace_Cache& operator+=(const Freespace_Cache &rhs)
	{
		auto p = base_, q = rhs.base_;
		while (p && q) {
			p->x_[0] += q->x_[0];
			p->x_[1] += q->x_[1];
			p->x_[2] += q->x_[2];
			p->x_[3] += q->x_[3];
			p = p->prev_;
			q = q->prev_;
		}

		if (!p && q) {
			copy_forward(q);
		}

		p = base_->next_, q = rhs.base_->next_;
		while (p && q) {
			p->x_[0] += q->x_[0];
			p->x_[1] += q->x_[1];
			p->x_[2] += q->x_[2];
			p->x_[3] += q->x_[3];
			p = p->next_;
			q = q->next_;
		}

		//@ copy back digits and update tail
		if (!p && q) {
			copy_backward(q);
		}

		carry();

		return *this;
	}

	Freespace_Cache& operator+=(IntType x)
	{
		base_->x_[0] += x;
		carry();

		return *this;
	}

	virtual Freespace_Cache& operator*=(IntType x)
	{
		auto node = tail_;

		IntType carrier = 0;
		while (node) {
			node->x_[0] *= x;
			node->x_[0] += carrier;
			carrier = node->x_[0] / Freespace_Cache::R;
			node->x_[0] %= Freespace_Cache::R;

			node->x_[1] *= x;
			node->x_[1] += carrier;
			carrier = node->x_[1] / Freespace_Cache::R;
			node->x_[1] %= Freespace_Cache::R;

			node->x_[2] *= x;
			node->x_[2] += carrier;
			carrier = node->x_[2] / Freespace_Cache::R;
			node->x_[2] %= Freespace_Cache::R;

			node->x_[3] *= x;
			node->x_[3] += carrier;
			carrier = node->x_[3] / Freespace_Cache::R;
			node->x_[3] %= Freespace_Cache::R;

			node = node->prev_;
		}

		if (carrier) {
			IntType ints[4] { carrier, 0, 0, 0 };
			head_ = allocate(ints, nullptr, head_);
		}

		//!erase suffix zero
		node = tail_;
		while (node != base_
			&& node->x_[0] == 0 && node->x_[1] == 0
			&& node->x_[2] == 0 && node->x_[3] == 0) {
			node = node->prev_;
			deallocate(node->next_);
			precision_ -= 4;
		}
		tail_ = node;
		tail_->next_ = nullptr;

		return *this;
	}

	virtual Freespace_Cache& operator/=(IntType x)
	{
		Node dummy, *node = &dummy;
		dummy.next_ = head_;

		//!skip useless prefix zero
		while (node->next_) {
			if (node->x_[0] == 0 && node->x_[1] == 0
				&& node->x_[2] == 0 && node->x_[3] == 0) {
				node = node->next_;
			} else {
				break;
			}
		}

		//!do solid division
		IntType remainder = 0;
		while (node->next_) {
			node = node->next_;

			node->x_[3] += remainder;
			remainder = (node->x_[3] % x) * Freespace_Cache::R;
			node->x_[3] /= x;

			node->x_[2] += remainder;
			remainder = (node->x_[2] % x) * Freespace_Cache::R;
			node->x_[2] /= x;

			node->x_[1] += remainder;
			remainder = (node->x_[1] % x) * Freespace_Cache::R;
			node->x_[1] /= x;

			node->x_[0] += remainder;
			remainder = (node->x_[0] % x) * Freespace_Cache::R;
			node->x_[0] /= x;
		}

		while (remainder > 0 && precision_ < max_precision_) {
			IntType ints[4] = { 0, 0, 0, remainder };
			node->next_ = allocate(ints, node, nullptr);
			node = node->next_;

			remainder = (node->x_[3] % x) * Freespace_Cache::R;
			node->x_[3] /= x;

			node->x_[2] = remainder;
			remainder = (node->x_[2] % x) * Freespace_Cache::R;
			node->x_[2] /= x;

			node->x_[1] = remainder;
			remainder = (node->x_[1] % x) * Freespace_Cache::R;
			node->x_[1] /= x;

			node->x_[0] = remainder;
			remainder = (node->x_[0] % x) * Freespace_Cache::R;
			node->x_[0] /= x;

			precision_ += 4;
		}

		tail_ = node;

		if (node->x_[0] == 0 && node->x_[1] == 0
			&& node->x_[2] == 0 && node->x_[3] == 0) {
			while (node != head_) {
				node = node->prev_;
				deallocate(node->next_);
			}
			base_ = head_;
			tail_ = base_;
			head_->next_ = nullptr;
			precision_ = 0;
			return *this;
		}

		node = head_;
		while (node != base_
			&& node->x_[0] == 0 && node->x_[1] == 0
			&& node->x_[2] == 0 && node->x_[3] == 0) {
			node = node->next_;
			deallocate(node->prev_);
		}
		head_ = node;
		head_->prev_ = nullptr;

		return *this;
	}

	friend std::ostream& operator<<(std::ostream &os, const Freespace_Cache &rhs)
	{
		char *buffer = new char[rhs.precision_ * Freespace_Cache::N + 256];
		char format[32], *p = buffer, *q;
		sprintf(format, "%%0%dd", Freespace_Cache::N);
		auto node = rhs.head_;
		while (node != rhs.base_->next_) {
			sprintf(p, format, node->x_[3]);
			p += Freespace_Cache::N;
			sprintf(p, format, node->x_[2]);
			p += Freespace_Cache::N;
			sprintf(p, format, node->x_[1]);
			p += Freespace_Cache::N;
			sprintf(p, format, node->x_[0]);
			p += Freespace_Cache::N;
			node = node->next_;
		}
		if (node != nullptr) {
			*p++ = '.';
			while (node) {
				sprintf(p, format, node->x_[3]);
				p += Freespace_Cache::N;
				sprintf(p, format, node->x_[2]);
				p += Freespace_Cache::N;
				sprintf(p, format, node->x_[1]);
				p += Freespace_Cache::N;
				sprintf(p, format, node->x_[0]);
				p += Freespace_Cache::N;
				node = node->next_;
			}
		}
		while (*--p == '0');
		p[1] = '\0';
		q = p + 1;
		p = buffer - 1;
		while (*++p == '0');
		if (*p == '.') --p;
		os << p;
		// delete[] buffer;
		return os;
	}

protected:
	Node *freespace = nullptr;

	virtual Node* allocate(const IntType *x, Node *prev, Node *next)
	{
		constexpr IntType null_ints[4] { 0, 0, 0, 0 };
		const IntType *ints = x ? x : null_ints;

		if (freespace) {
			Node *node = freespace;
			freespace = freespace->next_;
			new(node) Node(ints[0], ints[1], ints[2], ints[3], prev, next);
			return node;
		} else {
			return new Node(ints[0], ints[1], ints[2], ints[3], prev, next);
		}
	}

	virtual void deallocate(Node *node)
	{
		node->next_ = freespace;
		freespace = node;
	}

	void copy_forward(const Node *rhs)
	{
		while (rhs) {
			head_ = allocate(rhs->x_, nullptr, head_);
			rhs = rhs->prev_;
		}
	}

	void copy_backward(const Node *rhs)
	{
		while (rhs && precision_ < max_precision_) {
			tail_ = allocate(rhs->x_, tail_, nullptr);
			rhs = rhs->next_;
			precision_ += 4;
		}
	}

	void carry()
	{
		auto node = tail_;
		IntType carrier = 0;

		while (node) {
			node->x_[0] += carrier;
			carrier = node->x_[0] / Freespace_Cache::R;
			node->x_[0] %= Freespace_Cache::R;

			node->x_[1] += carrier;
			carrier = node->x_[1] / Freespace_Cache::R;
			node->x_[1] %= Freespace_Cache::R;

			node->x_[2] += carrier;
			carrier = node->x_[2] / Freespace_Cache::R;
			node->x_[2] %= Freespace_Cache::R;

			node->x_[3] += carrier;
			carrier = node->x_[3] / Freespace_Cache::R;
			node->x_[3] %= Freespace_Cache::R;

			node = node->prev_;
		}

		if (carrier) {
			IntType ints[4] { carrier, 0, 0, 0 };
			head_ = allocate(ints, nullptr, head_);
		}
	}
};
