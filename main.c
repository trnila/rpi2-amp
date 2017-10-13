#include <FreeRTOS.h>
#include <task.h>
#include "tools/mailbox.h"

#include "api.h"
#include "mini-printf.h"

void serial(void *param) {
	char buffer[32];

    uart_init();

    for(;;) {
        int c = uart_read();

        log_msg("recv: %c\n", c);
		mini_snprintf(buffer, sizeof(buffer), "Received %c\r\n", c);
        uart_print(buffer);
    }
}

void task1(void* param) {
	int i = 0;
	log_msg("start %d\n", param);
	for(;;) {
		log_msg("TICK[%d]: %d\n",  (int) param, i++);
		delay((int) param);
	}
}

void task2(void* param) {
	int i = 0;
	log_msg("start %d\n", param);
	for(;;) {
		log_msg("Different code[%d]: %d\n",  (int) param, i++);
		delay((int) param);
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
		delay(conf->delayOn);
		log_msg("LED %d %d\n", conf->pin, digitalRead(conf->pin));

		digitalWrite(conf->pin, LOW);
		delay(conf->delayOff);
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
	for(int i = 0; i < 4; i++) {
		mailbox_enable(i);
	}

	//create_task(task1, "TASK_1", (void*) 1000);
	//create_task(task1, "TASK_2", (void*) 1000);
	//create_task(task2, "TASK_3", (void*) 1000);

	TaskLedConfig ledConf[] = {
		{
			.pin = 2,
			.delayOn = 300,
			.delayOff = 350,
			.initialDelay = 0,
		},
		{
			.pin = 21,
			.delayOn = 1000,
			.delayOff = 1000,
			.initialDelay = 0,
		}
	};
	create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[0]);
	//create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[1]);
	//create_task(serial, "SERIAL", 0);

	vTaskStartScheduler();

	panic("PANIC: scheduler stopped\n");
}

void vApplicationMallocFailedHook(void) {
	panic("malloc failed");
}
