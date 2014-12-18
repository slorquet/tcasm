#arm assembly

.text
.thumb

mov r0, r1

mov sp, lr

add pc, #10

ldr r0, [r1, #0]
ldr r2, [r1, #0x24]

ldr r2, [r1, r0]

push {r0,r1,r4,lr}

#incorrect
push {r0,#42}

