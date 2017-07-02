#define WRITE(dst, val) *((int*) dst) = val

void c_irq_handler() {
	// increment number to we can see that irq handler has been triggered
	(*((int*) 0x30000400))++;


	WRITE(0x400000FC, 0xffffffff);
}
