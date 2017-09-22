#include "portmacro.h"

typedef struct {
	uword GPFSEL[6];
	uword Reserved_1;
 	uword GPSET[2];
	uword Reserved_2;
	uword GPCLR[2];
	uword Reserved_3;
	uword GPLEV[2];
	uword Reserved_4;
	uword GPEDS[2];
	uword Reserved_5;
	uword GPREN[2];
	uword Reserved_6;
	uword GPFEN[2];
	uword Reserved_7;
	uword GPHEN[2];
	uword Reserved_8;
	uword GPLEN[2];
	uword Reserved_9;
	uword GPAREN[2];
	uword Reserved_A;
	uword GPAFEN[2];
	uword Reserved_B;
	uword GPPUD[1];
	uword GPPUDCLK[2];
	//Ignoring the reserved and test bytes
} GPIO_BASE_ADDR;

volatile GPIO_BASE_ADDR *io = (GPIO_BASE_ADDR*) 0x3f200000;

void pinMode(int pin, int fn) {
	const int perReg = 10;
	const int fnWidth = 0x7; // function can take up to 3bits

	int offset = pin / perReg;
	io->GPFSEL[offset] |= (fn & fnWidth) << (3 * (pin - offset * perReg));
}

void digitalWrite(int pin, int val) {
	int offset = pin / 32;

	if(val) {
		io->GPSET[offset] = 1 << pin;
	} else {
		io->GPCLR[offset] = 1 << pin;
	}
}

int digitalRead(int pin) {
	int offset = pin / 32;
	return (io->GPLEV[offset] & (1 << pin)) > 0;
}