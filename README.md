tcasm
=====

Tiny Compact Assembler

This is a really tiny assembler, with the goal of using it in nuttx.

The primary target is ARM, more will follow if the project survives.

each text line is parsed independently and affects the global assembler state

the lines have the following syntax:

[label:] .directive [params...] @continuation-comment
[label:] mnemonic [operands...] @continuation-comment
#line comment

This is the gas syntax, as described here:
https://sourceware.org/binutils/docs/as/Statements.html#Statements
only one label per line is supported.

So here are the rules:

space at the beginning of the line is not significant.
line comments start with a #, the full line is ignored
continuation comments start with a @, the end of the line is ignored
only the N first characters of a line are parsed, usually N=80
directives starts with a dot
labels ends with a colon
labels must not start with a number.
(if the first word of a line does not ends with a colon, then it's a mnemonic.)


generic directives are parsed by common code:
.section <name> .text .data .bss .rodata
.ascii .asciiz
.db .dw .ds
.global .extern

other directives are parsed by the code generator
mnemonics are handled by the code generator
labels are handled by common code

current limitations that will be upgraded in the future

no cpp-like preprocessing
no file  inclusion
no macros
only one line/continuation comment char

-- slorquet

