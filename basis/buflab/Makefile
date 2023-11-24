PREFIX  ?=
CC      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

TARGETS := smoke fizz bang boom kaboom

TARGET  ?= smoke
STUID	?= 2021302468
BOMB    := bufbomb
ARGS    ?=

all: $(patsubst %,%.bin,$(TARGETS))
all: $(patsubst %,%.ans,$(TARGETS))
.PHONY: all

clean:
	@rm -rf *.bin *.ans
.PHONY: clean

%.o: %.s
	@$(CC) -c -o $@ $<

%.bin: %.o
	@$(OBJCOPY) -O binary -j .text $< $@

%.ans: %.bin
	@xxd -c 8 -g 1 $< | cut -d ' ' -f 2-9 > $@

qemu-run: $(TARGET).bin
	@qemu-i386 $(BOMB) -u $(STUID) $(ARGS) < $<
.PHONY: qemu-run

qemu-debug: $(TARGET).bin
	@qemu-i386 -g 1234 $(BOMB) -u $(STUID) $(ARGS) < $<
.PHONY: qemu-debug

monitor:
	@cd .. && make gdb TARGET=$(BOMB)
.PHONY: monitor
