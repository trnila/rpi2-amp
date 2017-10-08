#include <stddef.h>
#include "FreeRTOSConfig.h"

unsigned int __aeabi_idiv(unsigned int num, unsigned int den) {
	unsigned int quot = 0, qbit = 1;

	if (den == 0)
	{
		return 0;
	}

	/*
	 * left-justify denominator and count shift
	 */
	while ((signed int) den >= 0)
	{
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit)
	{
		if (den <= num)
		{
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	return quot;
}

void *memcpy(void *dest, const void *src, size_t n) {
	char *d = (char*) dest;
	char *s = (char*) src;

	while(n--) {
		*d = *s;
		d++;
		s++;
	}
}

void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function) {
	panic("%s:%d: %s Assertion %s failed.\n", file, line, function, assertion);
}