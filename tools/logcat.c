#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


int main() {
	int file = open("/dev/mem", O_RDONLY);
	if(file < 0) {
		perror("open");
		exit(1);
	}
	int offset = 0x30000000;


	int *addr = mmap(NULL, 1024, PROT_READ, MAP_SHARED, file, offset);
	if(!addr) {
		perror("mmap");
		exit(1);
	}

	close(file);

	printf("%s", addr);

	return 0;
}
