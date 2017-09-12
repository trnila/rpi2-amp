#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif
	typedef uint32_t TickType_t;

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR			char
#define portFLOAT			float
#define portDOUBLE		double
#define portLONG			long
#define portSHORT			short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	portLONG

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif
/*-----------------------------------------------------------*/	

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portBYTE_ALIGNMENT			8
#define portNOP()						__asm volatile ( "NOP" );
/*-----------------------------------------------------------*/	


/* Scheduler utilities. */

/*
 * portSAVE_CONTEXT, portRESTORE_CONTEXT, portENTER_SWITCHING_ISR
 * and portEXIT_SWITCHING_ISR can only be called from ARM mode, but
 * are included here for efficiency.  An attempt to call one from
 * THUMB mode code will result in a compile time error.
 */

#define portRESTORE_CONTEXT()															\
{																						\
	extern volatile void * volatile pxCurrentTCB;										\
	extern volatile unsigned portLONG ulCriticalNesting;								\
	log_msg("addr: %x\n", pxCurrentTCB); \
	__asm volatile( \
		"ldr r0, =pxCurrentTCB\n" \
		"ldr r1, [r0]\n" \
		"ldr sp, [r1]\n"\
		\
		"pop {r0-r12, r14}\n" \
		"ldr pc, [sp, #4]\n" \
		"ldr r0,=0x30000004\n" \
		"str sp, [r0]\n" \
		"bl fail\n" \
	); \
	log_msg("end"); \
}
/*-----------------------------------------------------------*/

#define portSAVE_CONTEXT()													\
{																			\
	extern volatile void * volatile pxCurrentTCB;							\
	extern volatile unsigned portLONG ulCriticalNesting;					\
																			\
	/* Push R0 as we are going to use the register. */						\
	__asm volatile (														\
	"STMDB	SP!, {R0}												\n\t"	\
																			\
	/* Set R0 to point to the task stack pointer. */						\
	"STMDB	SP,{SP}^	\n\t"	/* ^ means get the user mode SP value. */	\
	/*"NOP															\n\t" */ \
	"SUB	SP, SP, #4												\n\t"	\
	"LDMIA	SP!,{R0}												\n\t"	\
																			\
	/* Push the return address onto the stack. */							\
	"STMDB	R0!, {LR}												\n\t"	\
																			\
	/* Now we have saved LR we can use it instead of R0. */					\
	"MOV	LR, R0													\n\t"	\
																			\
	/* Pop R0 so we can save it onto the system mode stack. */				\
	"LDMIA	SP!, {R0}												\n\t"	\
																			\
	/* Push all the system mode registers onto the task stack. */			\
	"STMDB	LR,{R0-LR}^												\n\t"	\
	"NOP															\n\t"	\
	"SUB	LR, LR, #60												\n\t"	\
																			\
	/* Push the SPSR onto the task stack. */								\
	"MRS	R0, SPSR												\n\t"	\
	"STMDB	LR!, {R0}												\n\t"	\
																			\
	"LDR	R0, =ulCriticalNesting									\n\t"	\
	"LDR	R0, [R0]												\n\t"	\
	"STMDB	LR!, {R0}												\n\t"	\
																			\
	/* Store the new top of stack for the task. */							\
	"LDR	R0, =pxCurrentTCB										\n\t"	\
	"LDR	R0, [R0]												\n\t"	\
	"STR	LR, [R0]												\n\t"	\
	);																		\
	( void ) ulCriticalNesting;												\
	( void ) pxCurrentTCB;													\
}

extern void vTaskSwitchContext( void );
#define portYIELD_FROM_ISR()		vTaskSwitchContext()
#define portYIELD()					__asm volatile ( "SWI 0" )
/*-----------------------------------------------------------*/


/* Critical section management. */

/*
 * The interrupt management utilities can only be called from ARM mode.	 When
 * THUMB_INTERWORK is defined the utilities are defined as functions in 
 * portISR.c to ensure a switch to ARM mode.  When THUMB_INTERWORK is not 
 * defined then the utilities are defined as macros here - as per other ports.
 */

#ifdef THUMB_INTERWORK

	extern void vPortDisableInterruptsFromThumb( void ) __attribute__ ((naked));
	extern void vPortEnableInterruptsFromThumb( void ) __attribute__ ((naked));

	#define portDISABLE_INTERRUPTS()	vPortDisableInterruptsFromThumb()
	#define portENABLE_INTERRUPTS()	vPortEnableInterruptsFromThumb()
#error thumbmode
	
#else

	#define portDISABLE_INTERRUPTS()														\
			
	#define portENABLE_INTERRUPTS()														\

#endif /* THUMB_INTERWORK */

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portENTER_CRITICAL()		vPortEnterCritical();
#define portEXIT_CRITICAL()		vPortExitCritical();
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
