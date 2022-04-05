#pragma once

/**
 * @brief Partial inplement of big decimal based on double-linked list,
 *  designed for NOJ-002 PI calculation
 * @author zymelaii
 * @date 2022-03-20
 */

#include <cassert>
#include <type_traits>
#include <limits>
#include <initializer_list>
#include <sstream>
#include <iostream>

/**
 * @note Traits below are to restrain the Decimal template arguments
 */
namespace local_traits {
template <size_t x> struct pow10_t {
	static constexpr size_t value = 10 * pow10_t<x - 1>::value;
};
template <> struct pow10_t<1> {
	static constexpr size_t value = 10;
};
template <> struct pow10_t<0> {};

template <size_t x> struct pow10_square_t {
	static constexpr size_t pow10_value = pow10_t<x>::value;
	static constexpr size_t value = pow10_value * pow10_value;
};

template <size_t x> struct ispow10_t {
	static constexpr bool value = (x % 10 == 0) && ispow10_t<x / 10>::value;
};
template <> struct ispow10_t<1> {
	static constexpr bool value = true;
};
template <> struct ispow10_t<0> {
	static constexpr bool value = false;
};

/**
 * @note `pow10_square_t<U>::value - pow10_t<U>::value - 1` is the maximum number
 *  among implemented methods of Decimal, operation may cause overflow if the maximum
 *  is over integer limits
 */
template <class T, size_t U>
using Require = typename std::enable_if_t<
	std::is_unsigned<T>::value
	&& ispow10_t<pow10_t<U>::value>::value
	&& ispow10_t<pow10_square_t<U>::value>::value
	&& pow10_square_t<U>::value - pow10_t<U>::value - 1 < std::numeric_limits<T>::max(), int>;
};

/**
 * @brief Decimal class
 */
template <typename IntType, size_t NumPerUnit,
	local_traits::Require<IntType, NumPerUnit> = 0>
struct Decimal {
	static constexpr size_t  N = NumPerUnit; //@ each unit contains N decimal digits
	static constexpr IntType R = local_traits::pow10_t<NumPerUnit>::value; //@ pow(10, N)

	/**
	 * @brief Structure of decimal unit
	 */
	struct Node {
		Node *prev_, *next_;
		IntType x_;
		Node() {}
		Node(IntType x, Node *prev, Node *next)
			: x_(x), prev_(prev), next_(next)
		{
			if (prev) {
				prev->next_ = this;
			}
			if (next) {
				next->prev_ = this;
			}
		}
	};

	const size_t max_precision_; //@ enabled maximum precision of fractional part
	size_t precision_;   //@ current precision, i.e. number of units in fractional part
	Node *head_, *tail_; //@ pointer to head/tail unit
	Node *base_;         //@ pointer to unit right before the decimal point

	/**
	 * @note Disable default constructor
	 */
	Decimal() = delete;

	/**
	 * @brief Deep copy constructor
	 */
	Decimal(const Decimal &num)
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

	/**
	 * @brief Advanced ctor that supports decimal initialization
	 * @param precision: max precision, specifies the expected real length of fractional part
	 * @param fronts: units of integer part
	 * @param backs: units of fractional part
	 * @note: assume N=2, e.g.
	 *  1. 0.123124: Decimal(P, {0}, {12, 31, 24})
	 *  2. 12341: Decimal(P, {1, 23, 41})
	 *  3. 314.100001004: Decimal(P, {3, 14}, {10, 0, 1, 0, 40})
	 */
	Decimal(size_t precision,
		const std::initializer_list<IntType> &fronts = {},
		const std::initializer_list<IntType> &backs = {})
		: max_precision_((precision + Decimal::N - 1) / Decimal::N), precision_(0)
	{
		assert(fronts.size() || backs.size());

		auto it = fronts.begin();
		if (it == fronts.end()) {
			base_ = allocate(0, nullptr, nullptr);
			head_ = base_;
		} else {
			head_ = allocate(*it, nullptr, nullptr);
			base_ = head_;
			while (++it != fronts.end()) {
				base_ = allocate(*it, base_, nullptr);
			}
		}

		tail_ = base_;

		it = backs.begin();
		while (it != backs.end() && precision_ < max_precision_) {
			tail_ = allocate(*it, tail_, nullptr);
			++precision_;
			++it;
		}
	}

	~Decimal()
	{
		//@ gather allocated nodes and free them in one shot
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

	Decimal& operator+=(const Decimal &rhs)
	{
		//!do solid addition
		auto p = base_, q = rhs.base_;
		while (p && q) {
			p->x_ += q->x_;
			p = p->prev_;
			q = q->prev_;
		}

		//@ copy front digits and update head
		if (!p && q) {
			copy_forward(q);
		}

		p = base_->next_, q = rhs.base_->next_;
		while (p && q) {
			p->x_ += q->x_;
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

	Decimal& operator+=(IntType x)
	{
		base_->x_ += x;
		carry();

		return *this;
	}

	virtual Decimal& operator*=(IntType x)
	{
		auto node = tail_;

		//!do solid multiplication
		//@ use if-else case to avoid unnecessary division
		IntType carrier = 0;
		while (node) {
			node->x_ *= x;
			node->x_ += carrier;
			if (node->x_ >= Decimal::R) {
				carrier = node->x_ / Decimal::R;
				node->x_ %= Decimal::R;
			} else {
				carrier = 0;
			}
			node = node->prev_;
		}

		if (carrier) {
			head_ = allocate(carrier, nullptr, head_);
		}

		//!erase suffix zero
		node = tail_;
		while (node != base_ && node->x_ == 0) {
			node = node->prev_;
			deallocate(node->next_);
			--precision_;
		}
		tail_ = node;
		tail_->next_ = nullptr;

		return *this;
	}

	virtual Decimal& operator/=(IntType x)
	{
		Node dummy, *node = &dummy;
		dummy.next_ = head_;

		//!skip useless prefix zero
		while (node->next_) {
			if (node->next_->x_ == 0) {
				node = node->next_;
			} else {
				break;
			}
		}

		//!do solid division
		IntType remainder = 0;
		while (node->next_) {
			node = node->next_;
			node->x_ += remainder;
			remainder = (node->x_ % x) * Decimal::R;
			node->x_ /= x;
		}

		//!do soft division
		while (remainder > 0 && precision_ < max_precision_) {
			node->next_ = allocate(remainder, node, nullptr);
			node = node->next_;
			remainder = (node->x_ % x) * Decimal::R;
			node->x_ /= x;
			++precision_;
		}

		tail_ = node;

		/**
		 * @note `tail_->x_ == 0` means that the decimal reaches 0
		 * 	under the given precision and it should be reset to zero
		 */
		if (tail_->x_ == 0) {
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

		//!erase prefix zero
		node = head_;
		while (node != base_ && node->x_ == 0) {
			node = node->next_;
			deallocate(node->prev_);
		}
		head_ = node;
		head_->prev_ = nullptr;

		return *this;
	}

	/**
	 * @brief Support output for decimal
	 * @note Use buffer to accelerate convertion
	 */
	friend std::ostream& operator<<(std::ostream &os, const Decimal &rhs)
	{
		std::stringstream ss;

		char format[32], buf[32];
		sprintf(format, "%%0%dd", Decimal::N);

		auto node = rhs.head_;
		ss << node->x_;
		node = node->next_;

		while (node != rhs.base_->next_) {
			sprintf(buf, format, node->x_);
			ss << buf;
			node = node->next_;
		}

		if (node) {
			ss << '.';

			while (node != rhs.tail_) {
				sprintf(buf, format, node->x_);
				ss << buf;
				node = node->next_;
			}

			IntType x = node->x_, n = Decimal::N;
			while (x % 10 == 0) {
				x /= 10;
				--n;
			}

			sprintf(format, "%%0%dd", n);
			sprintf(buf, format, x);
			ss << buf;
		}

		os << ss.str();
		return os;
	}

protected:
	/**
	 * @note Introduce freespace to speed up allocation procedure
	 */
	Node *freespace = nullptr;

	virtual Node* allocate(IntType x, Node *prev, Node *next)
	{
		if (freespace) {
			Node *node = freespace;
			freespace = freespace->next_;
			new(node) Node(x, prev, next);
			return node;
		} else {
			return new Node(x, prev, next);
		}
	}

	virtual void deallocate(Node *node)
	{
		node->next_ = freespace;
		freespace = node;
	}

	/**
	 * @note Copy functions are introduced to clarify the decimal-add procedure
	 */
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
			++precision_;
		}
	}

	/**
	 * @brief Do carry in add procedure
	 */
	void carry()
	{
		auto node = tail_->prev_;
		IntType carrier = tail_->x_ / Decimal::R;
		tail_->x_ %= Decimal::R;

		//@ use if-else case to avoid unnecessary division
		while (node) {
			node->x_ += carrier;
			if (node->x_ >= Decimal::R) {
				carrier = node->x_ / Decimal::R;
				node->x_ %= Decimal::R;
			} else {
				carrier = 0;
			}
			node = node->prev_;
		}

		//!append isolated carrier
		if (carrier) {
			head_ = allocate(carrier, nullptr, head_);
		}
	}
};
