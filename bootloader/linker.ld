MEMORY {
	ram : ORIGIN = 0x20000000, LENGTH = 0x200
	log : ORIGIN = 0x20000200, LENGTH = 0x100
}

SECTIONS {
	.text : { *(.text*) } > ram
	.bss : { *(.bss*) } > ram
	.rodata : { *(.rodata*) } > ram
	.data : { *(.data*) } > ram
	.log : { *(.message*) } > log
}
