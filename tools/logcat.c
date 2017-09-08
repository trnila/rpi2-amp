#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#define LOG_SIZE 4096

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("%s addr\n", argv[0]);
		return 1;
	}

	size_t position;
	if(sscanf(argv[1], "0x%zx", &position) != 1) {
		printf("Failed to parse address\n");
		exit(1);
	}

	int file = open("/dev/mem", O_RDONLY);
	if(file < 0) {
		perror("open");
		exit(1);
	}

	size_t offset = position & ~(sysconf(_SC_PAGESIZE) - 1);
	char *addr = mmap(NULL, 1024 * 1024 * 1024, PROT_READ, MAP_SHARED, file, offset);
	if(!addr) {
		perror("mmap");
		exit(1);
	}
	close(file);

	addr += position - offset;

	int i;
	for(i = 0; i < LOG_SIZE && *addr; i++) {
		printf("%c", *addr);
		addr++;
	}
	
	return 0;
}
