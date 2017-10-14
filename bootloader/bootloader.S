.equ	USR_mode,	0x10
.equ	FIQ_mode,	0x11
.equ	IRQ_mode,	0x12
.equ	SVC_mode,	0x13
.equ	HYP_mode,	0x1A
.equ	SYS_mode,	0x1F
.equ    MASK_mode,  0x1F
.equ	No_Int,		0xC0

.equ    ERROR_irq_undefined, 1
.equ    ERROR_irq_swi,       2
.equ    ERROR_irq_prefetch,  3
.equ    ERROR_irq_data,      4
.equ    ERROR_irq_unused,    5
.equ    ERROR_irq_irq,       6
.equ    ERROR_irq_fiq,       7

.macro log letter
	ldr r1, =\letter
	ldr r2, =msg_num
	str r1, [r2]
.endm

.section ".log", "a"
    msg_num: .byte 0x0


.section ".text.startup"
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
	irq_handler:        .word handle_irq
	fiq_handler:        .word handle_fiq

reset:
    // disable irq, fiq
    cpsid if

    mrs r0, cpsr            // load current processor status register to r0
    and r0, r0, #MASK_mode  // mask only processore more
    cmp r0, #HYP_mode       // compare with hyp mode
    bne continue_boot       // if not in HYP mode, skip returning

    // set saved processor mode to SVC
    mov r0, # No_Int | SVC_mode
    msr spsr_c, r0

    // leave HYP mode to saved mode
    adr lr, 1f
    .word 0xe12ef30e // msr elr_hyp, lr
    .word 0xe160006e // eret
    1:	nop

continue_boot:
	// set address of our vector table
	// B4.1.156 VBAR, Vector Base Address Register, Security Extensions in arm_architecture_reference_manual.pdf
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0 // write r0 to VBAR

initialize_addresses:
    // store core ID to r5
    mrc p15, 0, r5, c0, c0, 5 // MPIDR, Multiprocessor Affinity Register, PMSA
    and r5, r5, #0x7f         // 7 lower bits represents core ID

    // multiply coreID by 4
    mov r0, #4
    mul r5, r5, r0 // r5 = coreID * 4

    // r6 = core * 4 + core0_mailbox_ctl
    ldr r1, =0x40000050 // core0_mailbox_ctl
    add r6, r5, r1 // r6 = core * 4 + core0_mailbox_ctl

    // r7 = core * 4 + core0_interrupt_sources
    ldr r1, =0x40000060 // core0_interrupt_sources
    add r7, r5, r1 // r7 = core * 4 + core_interrupt_sources


    // r8 = core * 4 * 4 + core0_mailbox0_rdclr
    ldr r1, =0x400000C0 // core0_mailbox0_rdclr
    mul r5, r5, r0 // r5 = coreID * 4 * 4
    add r8, r5, r1

    // r6 - mailbox ctl for current core
    // r7 - core interrupt sources
    // r8 - core mailbox0 rdclr
main:
    // enable 0 mailbox
    ldr r1, =1
    str r1, [r6]

    loop:
        // wait for interrupt
        wfi

        // check if we received mailbox irq
        ldr r0, [r7]
        tst r0, # 1 << 4

        beq loop

    // load jump address from mailbox
    ldr r2, [r8]

    // ack mailbox
    ldr r1, =0xffffffff
    str r1, [r8]

    // jump to new code
    mov pc, r2

	// program shall not get here, if so, loop forever
	hang:
		b hang

handle_und:
	log ERROR_irq_undefined
	b hang

handle_swi:
    log ERROR_irq_swi
    b hang

handle_pref:
	log ERROR_irq_prefetch
	b hang

handle_data:
	log ERROR_irq_data
	b hang

handle_unu:
	log ERROR_irq_unused
	b hang

handle_irq:
	log ERROR_irq_irq
	b hang

handle_fiq:
	log ERROR_irq_fiq
	b hang
