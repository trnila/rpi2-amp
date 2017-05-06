#include <stdarg.h>
#include <stdio.h>


char *_log = (char*) 0x30000000;

int log_msg(const char* msg, ...) {
	va_list list;
	va_start(list, msg);

//	_log += vsprintf(_log, msg, list);
	while(*msg) {
		*_log = *msg;
		_log++;
		msg++;
	}

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
