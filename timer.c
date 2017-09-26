#include <stdint.h>

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