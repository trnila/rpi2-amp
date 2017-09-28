#define BIT(n) (1 << n)

#include "gpio.h"

#define REG(addr) (*((int*) addr))
#define BIT(n) (1 << n)

int log_msg(const char* msg, ...);
