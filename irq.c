#include "const.h"
#include "FreeRTOSConfig.h"
#include <stdint.h>
#include "portmacro.h"
#include "api.h"
#include "timer.h"
#include "uart.h"

void c_irq_handler( void ) __attribute__((interrupt("IRQ"), naked));

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

		// handle uart irqs
		for(;;) {
			unsigned int s = REG(AUX_MU_IIR_REG);
			if((s & 6) == 4) {
				shouldSwitch |= irq_uart_received();
			} else {
				break;
			}
		}

		int source = REG(CORE3_IRQ_SOURCE);
		for(int i = 0; i < 4; i++) {
			if(source & (1 << (4 + i))) {
				int data = REG(CORE3_MBOX0_RDCLR + i);
				log_msg("Triggered irq: %d %d\n", i, data);

				// ack mailbox by clearing all bits
				REG(CORE3_MBOX0_RDCLR + i) = 0xffffffff;

				timer_stop();

				// switch to SVC mode before returning to bootloader
				asm volatile("msr cpsr_c, %0":: "r"(0x13));

				void (*reset)() = 0x20000000;
				reset();
			}
		}

		uint32_t val = getTimerCtl();
		if(val & TIMER_ISTATUS) {
			timer_reset();
			shouldSwitch = xTaskIncrementTick();
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
