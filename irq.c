#include "const.h"

#define REG(addr) (*((int*) addr))

int get_ctrl();

void c_irq_handler() {
	// increment number to we can see that irq handler has been triggered
	REG(0x30000400)++;

	int source = REG(CORE3_IRQ_SOURCE);
	if(source & INT_SRC_MBOX3) {
		REG(CORE3_MBOX3_RDCLR) = 0xffffffff;
	} else {
		unsigned int val = get_ctrl();
		if(!(val & (1 << 2))) {
			REG(0x30000000) = val;
			//not in interrupt?
		} else {
//			set_ctrl(val | (1 << 1));
		}
	}
}
