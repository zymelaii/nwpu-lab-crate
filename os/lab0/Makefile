SOURCE_FILES := $(wildcard *.asm)
BOOT_BINS := $(patsubst %.asm,%.bin,$(SOURCE_FILES))

TARGET ?= boot

all: $(BOOT_BINS)

%.bin: %.asm
	@echo "Compiling $<"
	@nasm -o $@ $<

clean:
	@echo "Clean outputs"
	@rm -rf *.bin

run: $(TARGET).bin
	@echo "Running bootloader $(TARGET).bin"
	@qemu-system-i386 -boot order=c -drive file=$(TARGET).bin,format=raw -display curses

.PHONY: clean run
