.section ".text.startup"
.global _start
.global restoreContext
.global fail

.equ	USR_mode,	0x10
.equ	FIQ_mode,	0x11
.equ	IRQ_mode,	0x12
.equ	SVC_mode,	0x13
.equ	HYP_mode,	0x1A
.equ	SYS_mode,	0x1F
.equ	No_Int,		0xC0

.macro log letter
	ldr r1,=\letter
	ldr r2,=0x30000000
	str r1, [r2]
.endm

_start:
	ldr pc,reset_handler
	ldr pc,undefined_handler
	ldr pc,swi_handler
	ldr pc,prefetch_handler
	ldr pc,data_handler
	ldr pc,unused_handler
	ldr pc,irq_handler
	ldr pc,fiq_handler
	reset_handler:      .word reset
	undefined_handler:  .word handle_und
	swi_handler:        .word vPortYieldProcessor
	prefetch_handler:   .word handle_pref
	data_handler:       .word handle_data
	unused_handler:     .word handle_unu
	irq_handler:        .word c_irq_handler
	fiq_handler:        .word fail

reset:
	// arm is currently in HYP mode due to raspi firmware

	// set saved processor mode to SVC
	mov r2, # No_Int | SVC_mode
	msr spsr_c, r2

	// leave HYP mode to saved mode
	adr lr, 2f
	.word 0xe12ef30e /* msr elr_hyp, lr */
	.word 0xe160006e /* eret */
	2:	nop


	// set address of our vector table
	// B4.1.156 VBAR, Vector Base Address Register, Security Extensions in arm_architecture_reference_manual.pdf
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0 // write r0 to VBAR


	// set stacks for each mode we will use
	// TODO: how to specify address of these stacks?
	mov 	r2, # No_Int | IRQ_mode
	msr		cpsr_c, r2
	ldr     sp, =(790 * 1024 * 1024)

	mov 	r2, # No_Int | SYS_mode
	msr		cpsr_c, r2
	ldr     sp, =(760 * 1024 * 1024)

	mov 	r2, # No_Int | SVC_mode
	msr		cpsr_c, r2
	ldr     sp, =(780 * 1024 * 1024)


	bl       kernel_main

	// program shall not get here, if so, loop forever
	loop:
		b loop

.globl get_ctrl
get_ctrl:
	MRC p15, 0, r0, c14, c2, 1
	bx lr

.globl set_ctrl
set_ctrl:
	MCR p15, 0, r0, c14, c2, 1
	bx lr

.globl PUT32
PUT32:
	str r1,[r0]
	bx lr

.globl GET32
GET32:
	ldr r0,[r0]
	bx lr

hang:
	b hang

handle_und:
	log 'a'
	b hang

handle_pref:
	log 'c'
	b hang

handle_data:
	log 'd'
	b hang

handle_unu:
	log 'e'
	b hang

fail:
	log '9'
	b fail

