MEMORY {
	ram : ORIGIN = 0x20008000, LENGTH = 0x4000000
}

SECTIONS {
	.text : { *(.text*) } > ram
	.bss : { *(.bss*) } > ram
	.rodata : { *(.rodata*) } > ram
	.data : { *(.data*) } > ram
}
