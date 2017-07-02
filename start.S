.section ".text.startup"
.global _start

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
	swi_handler:        .word handle_swi
	prefetch_handler:   .word handle_pref
	data_handler:       .word handle_data
	unused_handler:     .word handle_unu
	irq_handler:        .word irq
	fiq_handler:        .word irq

reset:
	log '0'
	wait:
		ldr r1,=0x39000000
		ldr r1, [r1]
		mov r2, #18
		cmp r1, r2
		bne wait

mrs r0, cpsr
eor r0, r0, #0x1a
tst r0, #0x1f
bic r0, r0, #0x1f
orr r0, r0, #0xd3
bne 1f
orr r0, r0, #0x100
adr lr, 2f
msr spsr_cxsf, r0
.word 0xe12ef30e /* msr elr_hyp, lr */
.word 0xe160006e /* eret */
1:	msr cpsr_c, r0
2:	nop


	log '1'

	// relocate vector table
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0
	
	// set stack
	mov 	r2, # No_Int | IRQ_mode
	msr		cpsr_c, r2
	ldr     sp, =(790 * 1024 * 1024)		//SP to 0x3000000

	mov 	r2, # USR_mode
	msr		cpsr_c, r2
	ldr     sp, =(768 * 1024 * 1024)		//SP to 0x3000000
	log '3'


	// enable interrupts
	mrs     r0, cpsr
	bic     r0, r0, #0x80
	msr     cpsr_c, r0
	log '7'

	// enable fiq
	mrs r0,cpsr
	bic r0,r0,#0x40
	msr cpsr_c,r0

	// enable 
	ldr r1,=(1 << 3)
	ldr r2,=0x40000010
	str r1, [r2]

	// enable all mailboxes
	ldr r1,=(15)
	ldr r2,=0x4000005C
	str r1, [r2]

	bl       kernel_main

	// program shall not get here, if so, loop forever
	loop:
		log 'A'
		b loop

irq:
	push	{r0, r1, r2, lr}

	# reset value in mailbox so irq handler will not repeat
	ldr r0, =0xffffffff
	ldr r1, =0x400000FC
	str r0, [r1]

	# increment value to debug that irq is running
	ldr r2,=0x30000300
	ldr r1,[r2]
	add r1, r1, #1
	str r1, [r2]

	pop		{r0, r1, r2, lr}
	subs	pc, lr, #4				@ return from exception


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

handle_swi:
	log 'b'
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
