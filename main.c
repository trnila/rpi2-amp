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
		log_msg("TICK: %d\n", i);
		vTaskDelay(1000);
	}
}


void kernel_main() {
	log_msg("initialized\n");

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	log_msg("We are lost\n");


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
