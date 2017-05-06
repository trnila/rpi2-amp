#include <stdarg.h>
#include <stdio.h>
#include "mini-printf.h"


char *_log = (char*) 0x30000000;

int log_msg(const char* msg, ...) {
	va_list list;
	va_start(list, msg);

	_log += mini_vsnprintf(_log, 128, msg, list);

	*_log = '\n'; _log++;
	*_log = 0;

	va_end(list);
}

/*
int main() {
	char x[1200];
	_log = &x;
	log_msg("Hello %d", 18);
	log_msg("World");
	printf("%s", x);

}*/
