#pragma once

#define INPUT 0
#define OUTPUT 1

#define HIGH 1
#define LOW 0

void pinMode(int pin, int fn);
void digitalWrite(int pin, int val);
int digitalRead(int pin);