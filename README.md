tcasm
=====

Tiny Compact Assembler

This is a really tiny assembler, with the goal of using it in nuttx.

    The primary target is ARM, more will follow if the project survives.
    Each text line is parsed independently and affects the global assembler state

The lines have the following syntax:

    [label:] .directive [params...] @continuation-comment
    [label:] mnemonic [operands...] @continuation-comment
    #line comment

    This is close to the gas syntax, as described here:
    https://sourceware.org/binutils/docs/as/Statements.html#Statements
    Only one label per line is supported.

So here are the rules:

    space at the beginning of the line is not significant.
    line comments start with a #, the full line is ignored
    continuation comments start with a @, the end of the line is ignored
    only the N first characters of a line are parsed, usually N=80
    directives starts with a dot
    labels ends with a colon
    labels must not start with a number.
    (if the first word of a line does not ends with a colon, then it's a mnemonic.)
    multiple instructions per line are not supported
    for ARM, .align 0 is not equivalent to 4-byte boundaries

    generic directives are parsed by common code:
    [todo] .global .extern
    [todo] .float .single .double
    [done] .end
    [done] .align .balign .p2align <value>[,<fill>]
    [done] .section <unquoted_name> .text .data .bss .rodata
    [done] .ascii .asciz .string <quoted string>
    [done] .db .byte 
    [done] .dh .hword .short
    [done] .dw .word .int .long (target dependent size)
    [done] .ds .space <size>[,<fill=0>]
    [done] .incbin

    other directives are parsed by the code generator
    mnemonics are handled by the code generator
    labels are handled by common code

current limitations that will be upgraded in the future

    no cpp-like preprocessing
    no file  inclusion
    no macros
    only one line/continuation comment char

ARM
===

parsed addressing modes

    rN,pc,sp,lr
    #val
    [reg, reg]
    [reg, #val]
    {reg, ...}

-- slorquet

