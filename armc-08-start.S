.section ".text.startup"

.global _start

_start:
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

	// Set the stack pointer, which progresses downwards through memory
	// Set it at 64MB which we know our application will not crash into
	// and we also know will be available to the ARM CPU. No matter what
	// settings we use to split the memory between the GPU and ARM CPU
	ldr     sp, =(768 * 1024 * 1024)		//SP to 0x3000000

	bl       kernel_main

	// program shall not get here, if so, loop forever
	loop:
		b loop
