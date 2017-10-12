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

char data[10][100];

void task2(void* param) {
	int i = 0;
	log_msg("start %d\n", param);
	for(;;) {
		log_msg("Different code[%d]: %d\n",  (int) param, i);
		delay((int) param);

		mini_snprintf(data + i, 100, "hello %d", i);
		mailbox_send(0, 1, data + i);

		i = (i + 1) % 10;
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


void out(int val, int dataPin, int clkPin) {
	for(int i = 0; i < 8; i++) {
		digitalWrite(dataPin, !!(val & (1 << i)));
		digitalWrite(clkPin, HIGH);
		digitalWrite(clkPin, LOW);
	}
}

void display(void* arg) {
	const int dataPin = 21;
	const int clkPin = 20;
	const int latchPin = 16;

	pinMode(dataPin, OUTPUT);
	pinMode(clkPin, OUTPUT);
	pinMode(latchPin, OUTPUT);

	int col = 0;
	int row = 0;
	for(;;) {
		digitalWrite(latchPin, LOW);
		out(1 << row, dataPin, clkPin);
		out(0xff, dataPin, clkPin);
		out(~(1 << col), dataPin, clkPin);
		out(0xff, dataPin, clkPin);
		digitalWrite(latchPin, HIGH);

		delay(50);

		col++;
		if(col > 8) {
			col = 0;
			row++;
			if(row >= 8) {
				row = 0;
				log_msg("clr\n");
			}
		}
	}
}

void kernel_main() {
	//create_task(task1, "TASK_1", (void*) 1000);
	//create_task(task1, "TASK_2", (void*) 1000);
//	create_task(task2, "TASK_3", (void*) 10);
	create_task(display, "displ", 0);

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
//	create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[0]);
	//create_task(taskBlinkLed, "TASK_LED", (void*) &ledConf[1]);
	//create_task(serial, "SERIAL", 0);

	vTaskStartScheduler();

	panic("PANIC: scheduler stopped\n");
}

void vApplicationMallocFailedHook(void) {
	panic("malloc failed");
}
