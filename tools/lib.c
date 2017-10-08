#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

void* mapPhys(int position, int size) {
	int file = open("/dev/mem", O_RDWR | O_SYNC);
	if(file < 0) {
		return NULL;
	}

	size_t offset = position & ~(sysconf(_SC_PAGESIZE) - 1);
	char *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file, offset);
	if(!addr) {
		return NULL;
	}
	close(file);

	addr += position - offset;
	return addr;
}
