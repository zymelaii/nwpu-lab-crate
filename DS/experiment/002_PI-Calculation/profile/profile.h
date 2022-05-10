#pragma once

#include <stdio.h>
#include <chrono>

#define StartProfile(TestCase, Targets, ...)  \
do { \
	auto start_ = std::chrono::high_resolution_clock::now(); \
	do { Targets, ## __VA_ARGS__ } while(0); \
	auto end_ = std::chrono::high_resolution_clock::now(); \
	fprintf(stderr, "CASE [" #TestCase "] cost time: %.2fms\n", \
		std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_).count() * 1e-3f); \
} while(0)
