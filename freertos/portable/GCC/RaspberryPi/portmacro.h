#ifndef PORTMACRO_H
#define PORTMACRO_H

typedef uint32_t TickType_t;
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
	extern volatile void * volatile pxCurrentTCB;										\
	extern volatile unsigned portLONG ulCriticalNesting;								\
	/*log_msg("restoring: '%s' .addr: %x\n", ((char*)pxCurrentTCB) + 52,  pxCurrentTCB);*/ \
	__asm volatile( \
		"ldr r0, =pxCurrentTCB\n" \
		"ldr r1, [r0]\n" \
		"ldr sp, [r1]\n"\
		\
		"pop {r0-r12, r14}\n" \
		"RFEIA	sp!\n" \
	); \ 
}

#define portSAVE_CONTEXT()													\
{																			\
	extern volatile void * volatile pxCurrentTCB;							\
	extern volatile unsigned portLONG ulCriticalNesting;					\
/*	log_msg("savstoring: '%s' .addr: %x\n", ((char*)pxCurrentTCB) + 52,  pxCurrentTCB);*/ \
	__asm volatile( \
		"srsdb sp!, 0x1f\n" /*store lr and spsr to stack in sys_mode (0x1ff) */\
		"cps 0x1f\n" /* enter sys_mode */ \
		"push {r0-r12, r14}\n" \
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
	__asm volatile ( "CPSID i" );									\
	__asm volatile ( "DSB" );										\
	__asm volatile ( "ISB" );

#define portCPU_IRQ_ENABLE()										\
	__asm volatile ( "CPSIE i" );									\
	__asm volatile ( "DSB" );										\
	__asm volatile ( "ISB" );

#define portDISABLE_INTERRUPTS()  \
	portCPU_IRQ_DISABLE(); \
	__asm volatile (  \
		"STMDB	SP!, {R0}			\n\t"	/* Push R0.								*/ \
		"MRS	R0, CPSR			\n\t"	/* Get CPSR.							*/ \
		"ORR	R0, R0, #0xC0		\n\t"	/* Disable IRQ, FIQ.					*/ \
		"MSR	CPSR, R0			\n\t"	/* Write back modified value.			*/ \
		"LDMIA	SP!, {R0}\n" 				/* Pop R0.								*/ \
		\
		/* CNTPNS - enable interrupt for non-secure physical timer */ \
		"push {r1,r2}\n" \
		"ldr r1, =0x0\n" \
		"movw r2, 0x004C\n" \
		"movt r2, 0x4000\n" \
		"str r1, [r2]\n" \
		"pop {r1,r2}\n" \
	);


#define portENABLE_INTERRUPTS()  \
	portCPU_IRQ_ENABLE(); \
	__asm volatile (  \
		"STMDB	SP!, {R0}			\n\t"	/* Push R0.								*/ \
		"MRS	R0, CPSR			\n\t"	/* Get CPSR.							*/ \
		"BIC	R0, R0, #0xC0		\n\t"	/* Disable IRQ, FIQ.					*/ \
		"MSR	CPSR, R0			\n\t"	/* Write back modified value.			*/ \
		"LDMIA	SP!, {R0}\n" 				/* Pop R0.								*/ \
		\
		/* CNTPNS - enable interrupt for non-secure physical timer */ \
		"push {r1,r2}\n" \
		"ldr r1, =0x02\n" \
		"movw r2, 0x004C\n" \
		"movt r2, 0x4000\n" \
		"str r1, [r2]\n" \
		"pop {r1,r2}\n" \
	);

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
