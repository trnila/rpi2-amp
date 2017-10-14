#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"


int main(char argc, char** argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s 0xaddress path/to/firmware\n", argv[0]);
		return 1;
	}

	int targetAddr;
	char* valueStr = argv[1];
	if(strncmp("0x", valueStr, 2) == 0) {
		sscanf(valueStr + 2, "%x", &targetAddr);
	} else {
		targetAddr = atoi(valueStr);
	}
	char *firmwarePath = argv[2];

	FILE* firmware = fopen(firmwarePath, "r");
	if(!firmware) {
		perror("Could not open firmware");
		return 1;
	}

	fseek(firmware, 0, SEEK_END);
	int size = ftell(firmware);
	fseek(firmware, 0, SEEK_SET);

	char* target = mapPhys(targetAddr, size);
	if(!target) {
		perror("Could not map physical memory");
		return 1;
	}

	char buffer[4096];
	int n;
	while((n = fread(buffer, 1, sizeof(buffer), firmware)) > 0) {
		for(int i = 0; i < n; i++) {
			*target = buffer[i];
			target++;
		}
	}

	fclose(firmware);
}
