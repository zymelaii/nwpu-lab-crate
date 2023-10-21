gdb:
	@echo - Run GDB
	@gdb -ex 'set tdesc filename misc/gdb/target.xml' \
		 -ex 'source misc/gdb/connect-qemu.gdb' \
		 -ex 'connect-qemu' \
		 -ex 'layout asm' \
		 -ex 'c'

.PHONY: gdb
