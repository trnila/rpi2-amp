.section ".text.startup"

.equ	USR_mode,	0x10
.equ	FIQ_mode,	0x11
.equ	IRQ_mode,	0x12
.equ	SVC_mode,	0x13
.equ	HYP_mode,	0x1A
.equ	SYS_mode,	0x1F
.equ	No_Int,		0xC0

.macro log letter
	ldr r1,=\letter
	ldr r2,=0x30000004
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
	swi_handler:        .word fail
	prefetch_handler:   .word handle_pref
	data_handler:       .word handle_data
	unused_handler:     .word handle_unu
	irq_handler:        .word fail
	fiq_handler:        .word fail

reset:
	log 42

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

log 43

	// set address of our vector table
	// B4.1.156 VBAR, Vector Base Address Register, Security Extensions in arm_architecture_reference_manual.pdf
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0 // write r0 to VBAR

    ldr r0, =0x4000005c
    ldr r1, =1
    str r1, [r0]

    log 0x2e

    cpsid i
    log 0x2f
    wfi
    log 0x3f

    // ack mailbox
    ldr r0, =0x400000f0
    ldr r1, =0xffffffff
    str r1, [r0]

    // jump to new code
    ldr r0, =new_addr
    ldr pc, [r0]




	// program shall not get here, if so, loop forever
	loop:
		b loop

new_addr: .word 0x20008000

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

