#include "const.h"
#include "FreeRTOSConfig.h"
#include <stdint.h>
#include "portmacro.h"
#include "api.h"
#include "timer.h"

#define REG(addr) (*((int*) addr))

int get_ctrl();
void c_irq_handler( void ) __attribute__((interrupt("IRQ"), naked));
void vTickISR (unsigned int nIRQ, void *pParam);


void c_irq_handler() {
	asm volatile(
			// we will return to interrupted instruction
			"sub lr, lr, #4\n"

			// save lr and current program status register
			"push {lr}\n"
			"mrs lr, SPSR\n"
			"push {lr}\n"

			// switch to SYS mode
			"cps #0x1f\n"

			// backup all registers
			"push {r0-r12, lr}\n"
	);

	{
		int shouldSwitch = 0;

		int source = REG(CORE3_IRQ_SOURCE);
		if(source & INT_SRC_MBOX3) {
			// ack mailbox by clearing all bits
			REG(CORE3_MBOX3_RDCLR) = 0xffffffff;
			log_msg("mailbox received\n");
		} else {
			uint32_t val = getTimerCtl();
			if(val & TIMER_ISTATUS) {
				setTimerVal(200000);
				shouldSwitch = xTaskIncrementTick();
			} else {
				panic("unknown interrupt val: %d\n", val);
			}
		}

		if(shouldSwitch) {
			asm(
				// recover registers
				"pop {r0-r12,lr}\n"
				// switch back to IRQ mode
				"cps #0x12\n"

				"pop {lr}\n"
				// recover previous mode
				"msr spsr_cxsf, lr\n"
				"pop {lr}\n"
			);

			portSAVE_CONTEXT();
			vTaskSwitchContext();
			portRESTORE_CONTEXT();

		} else {
			asm(
				"pop {r0-r12,lr}\n"
				"cps #0x12\n"
				"pop {lr}\n"
				"msr spsr_cxsf, lr\n"
				"pop {lr}\n"
				"movs pc, lr\n"
			);
		}
	}
}
