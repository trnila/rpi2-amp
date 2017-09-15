/*
 *	Raspberry Pi Porting layer for FreeRTOS.
 */

#include "FreeRTOS.h"
#include "task.h"
#include <drivers/irq.h>

/* Constants required to setup the task context. */
#define portINITIAL_SPSR						( ( portSTACK_TYPE ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT						( ( portSTACK_TYPE ) 0x20 )
#define portINSTRUCTION_SIZE					( ( portSTACK_TYPE ) 4 )
#define portNO_CRITICAL_SECTION_NESTING			( ( portSTACK_TYPE ) 0 )

#define portTIMER_PRESCALE 						( ( unsigned long ) 0xF9 )


/* Constants required to setup the VIC for the tick ISR. */
#define portTIMER_BASE                    		( (unsigned long ) 0x2000B400 )

typedef struct _BCM2835_TIMER_REGS {
	unsigned long LOD;
	unsigned long VAL;
	unsigned long CTL;
	unsigned long CLI;
	unsigned long RIS;
	unsigned long MIS;
	unsigned long RLD;
	unsigned long DIV;
	unsigned long CNT;
} BCM2835_TIMER_REGS;

static volatile BCM2835_TIMER_REGS * const pRegs = (BCM2835_TIMER_REGS *) (portTIMER_BASE);

/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );

/* 
 * The scheduler can only be started from ARM mode, so 
 * vPortISRStartFirstSTask() is defined in portISR.c. 
 */
extern void vPortISRStartFirstTask( void );

/*-----------------------------------------------------------*/

/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
portSTACK_TYPE *pxOriginalTOS;

	pxOriginalTOS = pxTopOfStack;

	*pxTopOfStack = 0x42424242;
	pxTopOfStack--;

	/* To ensure asserts in tasks.c don't fail, although in this case the assert
	is not really required. */

	// current processor state -> sys mode
	*pxTopOfStack = 0x1f;
	pxTopOfStack--;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	//*pxTopOfStack = ( portSTACK_TYPE ) pxCode;// + portINSTRUCTION_SIZE;		
	*pxTopOfStack = pxCode; // R15 - PC
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0xaaaaaaaa;	/* R14 - LR */
//	pxTopOfStack--;	
//	*pxTopOfStack = ( portSTACK_TYPE ) pxOriginalTOS; /* Stack used when task starts goes in R13. SP */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x01010101;	/* R1 */
	pxTopOfStack--;	

	/* When the task starts it will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters; /* R0 */


	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
//	*pxTopOfStack = ( portSTACK_TYPE ) portINITIAL_SPSR;

//	if( ( ( unsigned long ) pxCode & 0x01UL ) != 0x00 )
//	{
		/* We want the task to start in thumb mode. */
//		*pxTopOfStack |= portTHUMB_MODE_BIT;
//		log_msg("thumb mode\n");
//	}

//	pxTopOfStack--;

	/* Some optimisation levels use the stack differently to others.  This 
	means the interrupt flags cannot always be stored on the stack and will
	instead be stored in a variable, which is then saved as part of the
	tasks context. */
//	*pxTopOfStack = portNO_CRITICAL_SECTION_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	vPortISRStartFirstTask();	

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/
/*
 *	This is the TICK interrupt service routine, note. no SAVE/RESTORE_CONTEXT here
 *	as thats done in the bottom-half of the ISR.
 *
 *	See bt_interrupts.c in the RaspberryPi Drivers folder.
 */
void vTickISR (unsigned int nIRQ, void *pParam)
{
	xTaskIncrementTick();
	log_msg("in tick\n");

	#if configUSE_PREEMPTION == 1
	vTaskSwitchContext();
	#endif

//	pRegs->CLI = 0;			// Acknowledge the timer interrupt.
}

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
	portDISABLE_INTERRUPTS();

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
/*-----------------------------------------------------------*/

