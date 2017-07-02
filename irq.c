#define REG(addr) (*((int*) addr))

void c_irq_handler() {
	// increment number to we can see that irq handler has been triggered
	REG(0x30000400)++;


	REG(0x400000FC) = 0xffffffff;
}
