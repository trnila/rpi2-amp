#ifndef PORTMACRO_H
#define PORTMACRO_H
#include <stdint.h>

typedef uint32_t TickType_t;
typedef uint32_t uword;
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

// Architecture specifics
#define portSTACK_GROWTH			( -1 )
#define portBYTE_ALIGNMENT			8
#define portNOP()						__asm volatile ( "NOP" );

#define portRESTORE_CONTEXT()															\
{																						\
	__asm volatile( \
		"ldr r0, =pxCurrentTCB\n" \
		"ldr r1, [r0]\n" \
		"ldr sp, [r1]\n"\
		\
		"ldr r0, =ulCriticalNesting\n" \
		"pop {r1}\n" \
		"str r1, [r0]\n" \
		\
		"pop {r0-r12, r14}\n" \
		"RFEIA	sp!\n" \
	); \
}

#define portSAVE_CONTEXT()													\
{																			\
	__asm volatile( \
		"srsdb sp!, 0x1f\n" /*store lr and spsr to stack in sys_mode (0x1ff) */\
		"cps 0x1f\n" /* enter sys_mode */ \
		"push {r0-r12, r14}\n" \
		\
		"ldr r2, =ulCriticalNesting\n" \
		"ldr r1, [r2]\n" \
		"push {r1}\n" \
		\
		"ldr r0, =pxCurrentTCB\n" \
		"ldr r1, [r0]\n" \
		"str sp, [r1]\n" \
	); \
}


extern void vTaskSwitchContext( void );
#define portYIELD_FROM_ISR()		vTaskSwitchContext()
#define portYIELD()					__asm volatile ( "SWI 0" )

#define portCPU_IRQ_DISABLE()										\
	__asm volatile ( "CPSID if\n" );									

#define portCPU_IRQ_ENABLE()										\
	__asm volatile ( "CPSIE if\n" );									

#define portDISABLE_INTERRUPTS()  \
	portCPU_IRQ_DISABLE();

#define portENABLE_INTERRUPTS()  \
	portCPU_IRQ_ENABLE(); 
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portENTER_CRITICAL()		vPortEnterCritical();
#define portEXIT_CRITICAL()		vPortExitCritical();

// Task function macros as described on the FreeRTOS.org WEB site.
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#endif /* PORTMACRO_H */
