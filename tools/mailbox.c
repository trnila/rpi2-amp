#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "lib.h"

#define PERIPH_BASE 0x40000000
#define PERIPH_SIZE 4096

#define MAILBOX_SET_OFFSET 0x80
#define MAILBOX_RDCLR_OFFSET 0xC0

#if BAREMETAL == 1
static volatile uint32_t *io = (uint32_t*) PERIPH_BASE;
#else
unsigned int *io = NULL;

void mailbox_init() {
	io = (unsigned int*) mapPhys(PERIPH_BASE, PERIPH_SIZE); 
}
#endif

void mailbox_send(int core, int mailbox, unsigned int value) {
	assert(io != NULL);
	assert(core <= 3);
	assert(mailbox <= 3);


	*(io + (MAILBOX_SET_OFFSET + 16 * core + 4 * mailbox) / sizeof(int)) = value;
}

void mailbox_clear(int core, int mailbox) {
	assert(io != NULL);
	assert(core <= 3);
	assert(mailbox <= 3);

	*(io + (MAILBOX_RDCLR_OFFSET + 16 * core + 4 * mailbox) / sizeof(int)) = 0xffffffff;
}

unsigned int mailbox_read(int core, int mailbox) {
	assert(io != NULL);
	assert(core <= 3);
	assert(mailbox <= 3);

	return *(io + (MAILBOX_RDCLR_OFFSET + 16 * core + 4 * mailbox) / sizeof(int));
}
