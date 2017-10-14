#include <stdint.h>
#include "timer.h"

uint32_t freq;

uint32_t getTimerCtl() {
	uint32_t val;
	asm volatile("mrc p15, 0, %0, c14, c2, 1": "=r"(val));
	return val;
}

void setTimerCtl(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c14, c2, 1":: "r"(val));
}

void setTimerVal(uint32_t val) {
	asm volatile("mcr p15, 0, %0, c14, c2, 0" :: "r"(val));
}

uint32_t getTimerFreq() {
	uint32_t freq;
	asm volatile("MRC p15, 0, %0, c14, c0, 0": "=r"(freq));
	return freq;
}

void delay(const int ms) {
	vTaskDelay(ms);
}

void timer_init() {
	freq = getTimerFreq();

	// XXX: when we just OR 0 bit, it wont work after hard reboot
	setTimerCtl(TIMER_ENABLE);
	setTimerVal(freq);
}

void timer_stop() {
	setTimerCtl(0);
}

void timer_reset() {
	// tick equals to 1ms
	setTimerVal(freq / 1000);
}