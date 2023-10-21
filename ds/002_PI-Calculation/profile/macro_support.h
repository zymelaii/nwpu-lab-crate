#pragma once

#include "profile.h"

#define RunTarget(Target) \
{ \
	Target<uint64_t, 9> pi(n, { 2 }); \
	auto delta = pi; \
	delta /= 3; \
	for (int i = 2; delta.precision_ != 0; ++i) { \
		pi += delta; \
		delta *= i; \
		delta /= i * 2 + 1; \
	} \
	std::stringstream ss; \
	ss << pi; \
	std::cout << ss.str().substr(0, n + 2); \
}

#define RunTargetSilent(Target) \
{ \
	Target<uint64_t, 9> pi(n, { 2 }); \
	auto delta = pi; \
	delta /= 3; \
	for (int i = 2; delta.precision_ != 0; ++i) { \
		pi += delta; \
		delta *= i; \
		delta /= i * 2 + 1; \
	} \
	std::stringstream ss; \
	ss << pi; \
	auto result = ss.str().substr(0, n + 2); \
}

#define Profile(Target) \
StartProfile(Target, RunTarget(Target))

#define DecimalSpec(ThisSpec) \
template <typename IntType, size_t NumPerUnit, \
	local_traits::Require<IntType, NumPerUnit> = 0> \
struct ThisSpec : public Decimal<IntType, NumPerUnit>

#define DecimalInit(ThisSpec) \
using Decimal = Decimal<IntType, NumPerUnit>; \
using Node = typename Decimal::Node; \
ThisSpec(size_t precision, \
	const std::initializer_list<IntType> &fronts = {}, \
	const std::initializer_list<IntType> &backs = {}) \
	: Decimal(precision, fronts, backs) {}
