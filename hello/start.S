.section .text
.global _start

_start:
	ldr r1, ='O'
	ldr r2, =0x30000000
	str r1, [r2]

loop:
	b loop
