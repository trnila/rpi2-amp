#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#define SIZE 1024 * 1024 * 1024

int main(int argc, char **argv) {
	int file = open("/dev/mem", O_RDONLY);
	if(file < 0) {
		perror("open");
		exit(1);
	}

	char *addr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, file, 0x20000000);
	if(!addr) {
		perror("mmap");
		exit(1);
	}
	close(file);

	int i;
	for(i = 0; i < SIZE; i++) {
		*addr = 0;
		addr++;
	}
	
	return 0;
}
