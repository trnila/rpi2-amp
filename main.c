#include <FreeRTOS.h>
#include <task.h>

#include "api.h"
#include "io.h"

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

	create_task(task1, "TASK_1", (void*) 8);
	create_task(task1, "TASK_2", (void*) 10);
	create_task(task2, "TASK_3", (void*) 16);

	vTaskStartScheduler();

	panic("PANIC: scheduler stopped\n");
}

void vApplicationMallocFailedHook(void) {
	panic("malloc failed");
}
