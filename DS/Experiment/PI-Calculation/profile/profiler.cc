#include "pi_template.cc"
#include "profile.h"
#include "macro_support.h"
#include "pi_cache.cc"

DecimalSpec(Plain) {
DecimalInit(Plain);
public:
	Plain& operator*=(IntType x) override
	{
		auto node = this->tail_;
		IntType carrier = 0;
		while (node) {
			node->x_ *= x;
			node->x_ += carrier;
			carrier = node->x_ / Decimal::R;
			node->x_ %= Decimal::R;
			node = node->prev_;
		}
		if (carrier) {
			this->head_ = this->allocate(carrier, nullptr, this->head_);
		}
		node = this->tail_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->prev_;
			this->deallocate(node->next_);
			--this->precision_;
		}
		this->tail_ = node;
		this->tail_->next_ = nullptr;
		return *this;
	}
protected:
	Node* allocate(IntType x, Node *prev, Node *next) override
	{
		return new Node(x, prev, next);
	}
	void deallocate(Node *node) override
	{
		delete node;
	}
};

DecimalSpec(Freespace) {
DecimalInit(Freespace);
public:
	Freespace& operator*=(IntType x) override
	{
		auto node = this->tail_;
		IntType carrier = 0;
		while (node) {
			node->x_ *= x;
			node->x_ += carrier;
			carrier = node->x_ / Decimal::R;
			node->x_ %= Decimal::R;
			node = node->prev_;
		}
		if (carrier) {
			this->head_ = this->allocate(carrier, nullptr, this->head_);
		}
		node = this->tail_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->prev_;
			this->deallocate(node->next_);
			--this->precision_;
		}
		this->tail_ = node;
		this->tail_->next_ = nullptr;
		return *this;
	}
};

DecimalSpec(BranchMul) {
DecimalInit(BranchMul);
protected:
	Node* allocate(IntType x, Node *prev, Node *next) override
	{
		return new Node(x, prev, next);
	}
	void deallocate(Node *node) override
	{
		delete node;
	}
};

DecimalSpec(BranchDiv) {
DecimalInit(BranchDiv);
public:
	BranchDiv& operator/=(IntType x) override
	{
		Node dummy, *node = &dummy;
		dummy.next_ = this->head_;
		while (node->next_) {
			if (node->next_->x_ == 0) {
				node = node->next_;
			} else {
				break;
			}
		}
		IntType remainder = 0;
		while (node->next_) {
			node = node->next_;
			node->x_ += remainder;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
		}
		while (remainder > 0 && this->precision_ < this->max_precision_) {
			node->next_ = this->allocate(remainder, node, nullptr);
			node = node->next_;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
			++this->precision_;
		}
		this->tail_ = node;
		if (this->tail_->x_ == 0) {
			while (node != this->head_) {
				node = node->prev_;
				this->deallocate(node->next_);
			}
			this->base_ = this->head_;
			this->tail_ = this->base_;
			this->head_->next_ = nullptr;
			this->precision_ = 0;
			return *this;
		}
		node = this->head_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->next_;
			this->deallocate(node->prev_);
		}
		this->head_ = node;
		this->head_->prev_ = nullptr;
		return *this;
	}
protected:
	Node* allocate(IntType x, Node *prev, Node *next) override
	{
		return new Node(x, prev, next);
	}
	void deallocate(Node *node) override
	{
		delete node;
	}
};

DecimalSpec(Freespace_BranchMul) {
DecimalInit(Freespace_BranchMul);
public:
	Freespace_BranchMul& operator*=(IntType x) override
	{
		auto node = this->tail_;
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
			this->head_ = this->allocate(carrier, nullptr, this->head_);
		}
		node = this->tail_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->prev_;
			this->deallocate(node->next_);
			--this->precision_;
		}
		this->tail_ = node;
		this->tail_->next_ = nullptr;
		return *this;
	}
};

DecimalSpec(Freespace_BranchDiv) {
DecimalInit(Freespace_BranchDiv);
public:
	Freespace_BranchDiv& operator/=(IntType x) override
	{
		Node dummy, *node = &dummy;
		dummy.next_ = this->head_;
		while (node->next_) {
			if (node->next_->x_ == 0) {
				node = node->next_;
			} else {
				break;
			}
		}
		IntType remainder = 0;
		while (node->next_) {
			node = node->next_;
			node->x_ += remainder;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
		}
		while (remainder > 0 && this->precision_ < this->max_precision_) {
			node->next_ = this->allocate(remainder, node, nullptr);
			node = node->next_;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
			++this->precision_;
		}
		this->tail_ = node;
		if (this->tail_->x_ == 0) {
			while (node != this->head_) {
				node = node->prev_;
				this->deallocate(node->next_);
			}
			this->base_ = this->head_;
			this->tail_ = this->base_;
			this->head_->next_ = nullptr;
			this->precision_ = 0;
			return *this;
		}
		node = this->head_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->next_;
			this->deallocate(node->prev_);
		}
		this->head_ = node;
		this->head_->prev_ = nullptr;
		return *this;
	}
};

DecimalSpec(Freespace_BranchMul_BranchDiv) {
DecimalInit(Freespace_BranchMul_BranchDiv);
public:
	Freespace_BranchMul_BranchDiv& operator*=(IntType x) override
	{
		auto node = this->tail_;
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
			this->head_ = this->allocate(carrier, nullptr, this->head_);
		}
		node = this->tail_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->prev_;
			this->deallocate(node->next_);
			--this->precision_;
		}
		this->tail_ = node;
		this->tail_->next_ = nullptr;
		return *this;
	}

	Freespace_BranchMul_BranchDiv& operator/=(IntType x) override
	{
		Node dummy, *node = &dummy;
		dummy.next_ = this->head_;
		while (node->next_) {
			if (node->next_->x_ == 0) {
				node = node->next_;
			} else {
				break;
			}
		}
		IntType remainder = 0;
		while (node->next_) {
			node = node->next_;
			node->x_ += remainder;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
		}
		while (remainder > 0 && this->precision_ < this->max_precision_) {
			node->next_ = this->allocate(remainder, node, nullptr);
			node = node->next_;
			if (node->x_ != 0) {
				remainder = (node->x_ % x) * Decimal::R;
				node->x_ /= x;
			}
			++this->precision_;
		}
		this->tail_ = node;
		if (this->tail_->x_ == 0) {
			while (node != this->head_) {
				node = node->prev_;
				this->deallocate(node->next_);
			}
			this->base_ = this->head_;
			this->tail_ = this->base_;
			this->head_->next_ = nullptr;
			this->precision_ = 0;
			return *this;
		}
		node = this->head_;
		while (node != this->base_ && node->x_ == 0) {
			node = node->next_;
			this->deallocate(node->prev_);
		}
		this->head_ = node;
		this->head_->prev_ = nullptr;
		return *this;
	}
};

int main(int argc, char const *argv[])
{
	int n = 0;
	std::cin >> n;

#ifndef PROFILE_CACHE_ONLY
	Profile(Plain);
	Profile(Freespace);
	Profile(BranchMul);
	Profile(BranchDiv);
	Profile(Freespace_BranchMul);
	Profile(Freespace_BranchDiv);
	Profile(Freespace_BranchMul_BranchDiv);
#endif

	StartProfile(Freespace_Cache, {
		Freespace_Cache<uint64_t, 9> pi(n, 2);
		auto delta = pi;
		delta /= 3;
		for (int i = 2; delta.precision_ != 0; ++i) {
			pi += delta;
			delta *= i;
			delta /= i * 2 + 1;
		}
		std::stringstream ss;
		ss << pi;
		std::cout << ss.str().substr(0, n + 2);
	});

	return 0;
}
