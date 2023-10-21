define connect-qemu
python
import gdb
prog = gdb.selected_inferior()
if prog and prog.is_valid() and prog.connection:
    gdb.execute('kill')
end
target remote localhost:1234
end
