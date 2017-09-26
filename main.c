#include <FreeRTOS.h>
#include <task.h>

#include "api.h"

void serial(void *param);


void task1(void* param) {
	int i = 0;
	log_msg("start %d\n", param);
	for(;;) {
		log_msg("TICK[%d]: %d\n",  (int) param, i++);
//		taskYIELD();
		vTaskDelay((int) param);
	}
}

void task2(void* param) {
	int i = 0;
	log_msg("start %d\n", param);
	for(;;) {
		log_msg("Different code[%d]: %d\n",  (int) param, i++);
//		taskYIELD();
		vTaskDelay((int) param);
	}
}


typedef struct {
	int pin;
	int delayOn;
	int delayOff;
	int initialDelay;
} TaskLedConfig;

void taskBlinkLed(void *param) {
	TaskLedConfig* conf = (TaskLedConfig*) param;

	pinMode(conf->pin, OUTPUT);
	vTaskDelay(conf->initialDelay);

	for(;;) {
		digitalWrite(conf->pin, HIGH);
		vTaskDelay(conf->delayOn);
		log_msg("LED %d %d\n", conf->pin, digitalRead(conf->pin));

		digitalWrite(conf->pin, LOW);
		vTaskDelay(conf->delayOff);
		log_msg("LED %d %d\n", conf->pin, digitalRead(conf->pin));
	}
}

void create_task(TaskFunction_t fn, const char* name, void* param) {
	BaseType_t msg = xTaskCreate(fn, name, 128, param, 0, 0);
	if(msg != pdPASS) {
		if(msg == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
			panic("xtaskcreate failed: memory\n");
		} else {
			panic("xtaskcreate failed: unknown\n");
		}
	}
}

void kernel_main() {
	portDISABLE_INTERRUPTS();

	//create_task(task1, "TASK_1", (void*) 8);
	//create_task(task1, "TASK_2", (void*) 10);
	//create_task(task2, "TASK_3", (void*) 16);

	TaskLedConfig ledConf[] = {
		{
			.pin = 2,
			.delayOn = 8,
			.delayOff = 20,
			.initialDelay = 5,
		},
		{
			.pin = 21,
			.delayOn = 20,
			.delayOff = 8,
			.initialDelay = 0,
		}
	};
	//create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[0]);
	//create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[1]);
	create_task(serial, "SERIAL", 0);

	vTaskStartScheduler();

	panic("PANIC: scheduler stopped\n");
}

void vApplicationMallocFailedHook(void) {
	panic("malloc failed");
}
