.section ".text.startup"
.global _start

.equ    FIQSTACK3 , 21504
.equ    IRQSTACK3 , 22528
.equ    KSTACK3   , 38912
.equ    USTACK3   , 71680
.equ    FIQSTACK2 , 21248
.equ    IRQSTACK2 , 22272
.equ    KSTACK2   , 34816
.equ    USTACK2   , 63488
.equ    FIQSTACK1 , 20992
.equ    IRQSTACK1 , 22016
.equ    KSTACK1   , 30720
.equ    USTACK1   , 55296
.equ    FIQSTACK0 , 20736
.equ    IRQSTACK0 , 21760
.equ    KSTACK0   , 26624
.equ    USTACK0   , 47104


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
	undefined_handler:  .word loop
	swi_handler:        .word loop
	prefetch_handler:   .word loop
	data_handler:       .word loop
	unused_handler:     .word loop
	irq_handler:        .word irq
	fiq_handler:        .word irq

reset:
	log '0'
	// Clear CORE3_MBOX3
	ldr r1,=0x400000FC
	ldr r3,=0xffffffff
	str r3, [r1]

	wait:
		ldr r1,=0x39000000
		ldr r1, [r1]
		mov r2, #18
		cmp r1, r2
		bne wait

	log '1'

	// relocate vector table
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0

	mrc p15, 0, r0, c1, c0, 0 @ Read System Control Register
	@	orr r0, r0, #(1<<2)       @ dcache enable
		orr r0, r0, #(1<<12)      @ icache enable
			and r0, r0, #0xFFFFDFFF   @ turn on vector table at 0x0000000 (bit 12)
		mcr p15, 0, r0, c1, c0, 0 @ Write System Control Register
	
	log '2'
	
	
	ldr     sp, =(768 * 1024 * 1024)		//SP to 0x3000000


	
#	mov 	r2, # No_Int | IRQ_mode
#	msr		cpsr_c, r2
#	mov		sp, # IRQSTACK3

#	mov 	r2, # No_Int | FIQ_mode
#	msr		cpsr_c, r2
#	mov		sp, # FIQSTACK3

#	mov 	r2, # No_Int | HYP_mode
#	msr		cpsr_c, r2
#	mov		sp, # KSTACK3

	bl       kernel_main

	// program shall not get here, if so, loop forever
	loop:
		log 'A'
		b loop

irq:
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
//	bl c_irq_handler
	log 'B'
	pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	subs pc,lr,#4

.globl PUT32
PUT32:
	str r1,[r0]
	bx lr

.globl GET32
GET32:
	ldr r0,[r0]
	bx lr
