#include "FreeRTOS.h"
#include "task.h"
#include <drivers/irq.h>

#define portINITIAL_SPSR						( ( portSTACK_TYPE ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portNO_CRITICAL_SECTION_NESTING			( ( portSTACK_TYPE ) 0 )

volatile int shouldSwitch = 0;

static void prvSetupTimerInterrupt();
extern void vPortISRStartFirstTask();

void taskReturned() {
	portDISABLE_INTERRUPTS();
	panic("panic: task returned from main function\n");
}

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{

	// last value marker
	*pxTopOfStack = 0x42424242;
	pxTopOfStack--;

	// current processor state -> sys mode
	*pxTopOfStack = portINITIAL_SPSR;
	pxTopOfStack--;

	// R15 - PC
	// start instruction of the task
	*pxTopOfStack = (portSTACK_TYPE) pxCode; // R15 - PC
	pxTopOfStack--;

	// R14 - LR
	*pxTopOfStack = ( portSTACK_TYPE ) taskReturned;
	pxTopOfStack--;

	// R13 - SP
	// not on stack, value is stored in pxCurrentTCB

	// R12
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12121212;
	pxTopOfStack--;

	// R11
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;
	pxTopOfStack--;

	// R10
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10101010;
	pxTopOfStack--;

	// R9
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09090909;
	pxTopOfStack--;

	// R8
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08080808;
	pxTopOfStack--;

	// R7
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07070707;
	pxTopOfStack--;

	// R6
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06060606;
	pxTopOfStack--;

	// R5
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05050505;
	pxTopOfStack--;

	// R4
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04040404;
	pxTopOfStack--;

	// R3
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03030303;
	pxTopOfStack--;

	// R2
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02020202;
	pxTopOfStack--;

	// R1
	*pxTopOfStack = ( portSTACK_TYPE ) 0x01010101;
	pxTopOfStack--;

	// When the task starts it will expect to find the function parameter in R0
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
	pxTopOfStack--;

	*pxTopOfStack = 0; //portNO_CRITICAL_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	//  Start the timer that generates the tick ISR.  Interrupts are disabled
	// here already.
	prvSetupTimerInterrupt();

	// Start the first task.
	vPortISRStartFirstTask();

	panic("first task returned?\n");
	return 0;
}

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */

	panic("Scheduler stopped\n");
}

/*
 *	This is the TICK interrupt service routine, note. no SAVE/RESTORE_CONTEXT here
 *	as thats done in the bottom-half of the ISR.
 */
void vTickISR (unsigned int nIRQ, void *pParam)
{
	shouldSwitch = xTaskIncrementTick();
//	log_msg("in tick %d %d\n", xTaskGetTickCount(), shouldSwitch);

	#if configUSE_PREEMPTION == 1
	if(shouldSwitch) {
//		vTaskSwitchContext();
	}
	#endif
}

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{

	asm(
		// enable physical timer (CNTP_CTL, PL1 Physical Timer Control register)
		"ldr r1,=1\n" // enable timer (ENABLE)"
		"mcr p15, 0, r1, c14, c2, 1\n" // store value"

		// (CNTP_CVAL, PL1 Physical Timer CompareValue register)
		// holds compare value of timer
//		"ldr r1,=100\n" // least-significant"
//		"ldr r2,=0\n" // most-significant"
//		"MCRR p15, 2, r1, r2, c14\n"
	);
	asm("ldr r1, =80000000");
	asm("MCR p15, 0, r1, c14, c2, 0");
}

