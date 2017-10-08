#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mailbox.h"


int main(char argc, char** argv) {
	if(argc != 4) {
		fprintf(stderr, "Usage: %s core mailbox value\n", argv[0]);
		return 1;
	}

	int core = atoi(argv[1]);
	int mailbox = atoi(argv[2]);
	int value = 0;
	char* valueStr = argv[3];
	if(strncmp("0x", valueStr, 2) == 0) {
		sscanf(valueStr + 2, "%x", &value);
	} else {
		value = atoi(valueStr);
	}

	mailbox_init();
	mailbox_send(core, mailbox, value);
}
