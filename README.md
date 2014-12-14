tcasm
=====

Tiny Compact Assembler

This is a really tiny assembler, with the goal of using it in nuttx.

The primary target is ARM, more will follow if the project survives.

each text line is parsed independently and affects the global assembler state

the lines have the following syntax:

[label:] .directive [params...] #comment

[label:] mnemonic [operands...] #comment

This is the gas syntax, as described here:
https://sourceware.org/binutils/docs/as/Statements.html#Statements
only one label per line is supported.

So here are the rules:

space at the beginning of the line is not significant.
directives starts with a dot
labels ends with a colon and are followed by some spaces
labels must not start with a number.
if the first word of a line does not ends with a colon, then it's a mnemonic.


generic directives are parsed by common code:
.section <name> .text .data .bss .rodata
.ascii .asciiz
.db .dw .ds
.xref

other directives are parsed by the code generator

labels are handled by common code

mnemonics are handled by the code generator

current limitations that will be upgraded in the future

no cpp-like preprocessing
no file  inclusion
no macros
only one comment char
all symbols are global

-- slorquet

