#include "const.h"
#include "FreeRTOSConfig.h"
#include <stdint.h>
#include "portmacro.h"

#define REG(addr) (*((int*) addr))

int get_ctrl();
void c_irq_handler( void ) __attribute__((interrupt("IRQ"), naked));
extern volatile int shouldSwitch;
void vTickISR (unsigned int nIRQ, void *pParam);


void c_irq_handler() {
	asm(
			"sub lr, lr, #4\n"

			"push {lr}\n"
			"mrs lr, SPSR\n"
			"push {lr}\n"

			"cps #0x13\n"
			"push {r0-r12, lr}\n"
	);

	// increment number to we can see that irq handler has been triggered
	REG(0x30000400)++;

	int source = REG(CORE3_IRQ_SOURCE);
	if(source & INT_SRC_MBOX3) {
		REG(CORE3_MBOX3_RDCLR) = 0xffffffff;
		REG(0x30000440)++;
		REG(0x400000A8) = 42;
	} else {
		unsigned int val = get_ctrl();
		if(!(val & (1 << 2))) {
			panic("unknown interrupt val: %d\n", val);
		} else {
			// CNTP_TVAL, PL1 Physical TimerValue register,			
			vTickISR(64, 0);
			asm("ldr r1, =20000000");
			asm("MCR p15, 0, r1, c14, c2, 0");
		}
	}

	if(shouldSwitch) {
		asm(
			"cpsid i\n"
			"dsb\n"
			"isb\n"

			"pop {r0-r12,lr}\n"
			"cps #0x12\n"
			"pop {lr}\n"
			"msr spsr_cxsf, lr\n"
			"pop {lr}\n"
		);

		portSAVE_CONTEXT();
		vTaskSwitchContext();
		portRESTORE_CONTEXT();

	} else {
		asm(
			"cpsid i\n"
			"dsb\n"
			"isb\n"

			"pop {r0-r12,lr}\n"
			"cps #0x12\n"
			"pop {lr}\n"
			"msr spsr_cxsf, lr\n"
			"pop {lr}\n"
			"movs pc, lr\n"
		);
	}
}
