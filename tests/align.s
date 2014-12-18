.data

#create a 3-byte block
.db 42
.balign 3

.string "abc"
.balign 3

#create a 8-byte block
.db 23
.p2align 3

.db 0xFF
.align
.db 0xFF

