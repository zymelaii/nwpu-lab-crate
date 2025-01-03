SHELL := /bin/bash

K    ?= #<! total child threads
N    ?= #<! index of fib numbers to be calc
SYNC ?= #<! which sync primitive to use

DEFS :=
DEFS += NDEBUG
DEFS += $(if $(K),NR_THREADS=$(K),)
DEFS += $(if $(N),ARG_FIB_N=$(N),)
DEFS += $(if $(SYNC),SYNC_$(SYNC))

run: main.out
	@./$<

main.out: main.cpp
	@g++ $(addprefix -D,$(DEFS)) -O0 -lpthread -o $@ $<
