#include <stdarg.h>
#include <stdio.h>
#include "mini-printf.h"

#define LOG_SIZE 4096
char _log[LOG_SIZE];
int pos = 0;

int log_msg(const char* msg, ...) {
	char line[64];

	va_list list;
	va_start(list, msg);

	int size = mini_vsnprintf(line, sizeof(line), msg, list);
	va_end(list);

	char *r = line;
	do {
		_log[pos] = *r;
		pos = (pos + 1) % LOG_SIZE;
		r++;
	} while(*r);
}
