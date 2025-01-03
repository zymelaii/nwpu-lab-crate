SHELL := /bin/bash

K ?= #<! total openmp threads
N ?= #<! multiplier for total graph vertices, |V| = K * N
D ?= #<! graph density

ENABLE_TIMEOUT ?= #<! setup a 2000ms timeout for cpu run
NOCPU          ?= #<! disable cpu run
NOIO           ?= #<! disable io ops

DEFS :=
DEFS += NDEBUG
DEFS += $(if $(K),NR_THREADS=$(K),)
DEFS += $(if $(N),ARG_MULTIPLIER_N=$(N),)
DEFS += $(if $(D),ARG_DENSITY=$(D),)
DEFS += $(if $(ENABLE_TIMEOUT),ENABLE_TIMEOUT,)
DEFS += $(if $(NOCPU),NOCPU,)
DEFS += $(if $(NOIO),NOIO,)

run: main.out
	@./$<

main.out: main.cpp
	@g++ $(addprefix -D,$(DEFS)) -O0 -fopenmp -o $@ $<
