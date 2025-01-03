SHELL := /bin/bash

P ?= #<! total openmp threads
K ?= #<! total kmeans clusters
N ?= #<! total iterations

NOMT ?= #<! run in serial impl

DEFS :=
DEFS += NDEBUG
DEFS += $(if $(P),NR_THREADS=$(P),)
DEFS += $(if $(K),ARG_NR_CLUS=$(K),)
DEFS += $(if $(N),ARG_MAX_ITERS=$(N),)
DEFS += $(if $(NOMT),NOMT,)

run: main.out
	@./$<

main.out: main.cpp
	@g++ $(addprefix -D,$(DEFS)) -g -O0 -fopenmp -o $@ $<
