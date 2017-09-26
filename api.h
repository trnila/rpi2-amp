#include "gpio.h"

#define REG(addr) (*((int*) addr))
#define PUT32(a, b) REG(a) = b
#define GET32(a) REG(a)

int log_msg(const char* msg, ...);
