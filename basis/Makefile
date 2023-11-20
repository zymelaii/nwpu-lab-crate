TARGET ?=

gdb:
	@\
	if [ ! "$(TARGET)" ]; then \
		echo 'error: no executable specified'; \
	else \
		gdb -ex 'set tdesc filename misc/gdb/target.xml' \
			-ex 'source misc/gdb/connect-qemu.gdb' \
			-ex 'connect-qemu' \
			-ex 'layout asm' \
			-ex 'file $(TARGET)' \
			-ex "b main" \
			-ex 'c'; \
	fi
.PHONY: gdb
