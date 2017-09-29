#pragma once

// section 6.2 in BCM2835-ARM-Peripherals.pdf
#define INPUT 0
#define OUTPUT 1
#define ALT0 4
#define ALT1 5
#define ALT2 6
#define ALT3 7
#define ALT4 3
#define ALT5 2

#define NO_PULL 0
#define PULL_UP BIT(4)
#define PULL_DOWN BIT(5)

#define HIGH 1
#define LOW 0



void pinMode(int pin, int fn);
void digitalWrite(int pin, int val);
int digitalRead(int pin);