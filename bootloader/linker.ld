MEMORY {
	ram : ORIGIN = 0x20000000, LENGTH = 0x300
}

SECTIONS {
	.text : { *(.text*) } > ram
	.bss : { *(.bss*) } > ram
	.rodata : { *(.rodata*) } > ram
	.data : { *(.data*) } > ram
}
