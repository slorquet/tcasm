tcasm
=====

Tiny Compact Assembler

This is a really tiny assembler, with the goal of using it in nuttx.

The primary target is ARM, more will follow if the project survives.

each text line is parsed independently and affects the global assembler state

the lines have the following syntax:

.directive [params...]

[label:] mnemonic [operands...] #comment

so here are the rules:

directives starts with a dot
labels ends with a colon and are followed by some spaces
if the first word of a line does not contain a colon, then it's a mnemonic.

space at the beginning of the line is not significant.

generic directives are parsed by common code:
.section <name> .text .data .bss .rodata
.ascii .asciiz
.db .dw .ds
.xref

other directives are parsed by the code generator

labels are handled by common code

mnemonics are handled by the code generator

current limitations that will be upgraded in the future

no file  inclusion
no macros
only one comment char
all symbols are global

-- slorquet

