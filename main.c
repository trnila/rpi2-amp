#include <FreeRTOS.h>
#include <task.h>

#include "api.h"
#include "io.h"

volatile char delay = 0x54;

/** GPIO Register set */
volatile unsigned int* gpio;

/** Simple loop variable */
volatile unsigned int tim;
volatile unsigned int veces;

void task1(void* param) {
	int i = 0;
	for(;;) {
		log_msg("TICK[%d]: %d\n",  (int) param, i++);
//		taskYIELD();
		vTaskDelay(5);
	}
}

void task2(void* param) {
	int i = 0;
	for(;;) {
		log_msg("Different code[%d]: %d\n",  (int) param, i++);
//		taskYIELD();
		vTaskDelay(5);
	}
}

void create_task(TaskFunction_t fn, const char* name, void* param) {
	BaseType_t msg = xTaskCreate(fn, name, 128, param, 0, 0);
	if(msg != pdPASS) {
		if(msg == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
			log_msg("xtaskcreate failed: memory\n");
		} else {
			log_msg("xtaskcreate failed: unknown\n");
		}
		for(;;) {}
	}
}

void kernel_main() {
	portDISABLE_INTERRUPTS();

	create_task(task1, "TASK_1", 22);
	create_task(task1, "TASK_2", 44);
	create_task(task2, "TASK_3", 66);

	vTaskStartScheduler();

	log_msg("PANIC: scheduler stopped\n");
	for(;;) {}


	/* Assign the address of the GPIO peripheral (Using ARM Physical Address) */
	gpio = (unsigned int*)GPIO_BASE;

	/* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
	   peripheral register to enable GPIO16 as an output */
	gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

	int i = 0;
	for(;;) {

		for(tim = 0; tim < delay * 10000; tim++);

		/* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
		   for plus models )*/
		gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

		for(tim = 0; tim < delay * 10000; tim++);

		/* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
		   for plus models )*/
		gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
		log_msg("tick %d\n", i++);
	}
}

void vApplicationMallocFailedHook(void) {
	log_msg("malloc failed\n");
	for(;;) {}
}
