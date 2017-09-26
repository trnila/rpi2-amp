#include "api.h"
#include "const.h"

#define GPPUD       0x3f200094
#define GPPUDCLK0   0x3f200098


#define BUFFER_SIZE 32
char input[BUFFER_SIZE];

void irq_uart_received() {
    int c = REG(AUX_MU_IO_REG);
    log_msg("RECEIVED %c\n", c);
}

void serial(void *param) {

	const int txd = 14;
	const int rxd = 15;
	unsigned int ra=0;

	REG(AUX_ENABLES) |= 1; // enable mini uart

    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    PUT32(AUX_MU_IER_REG,5); // enable interrupts
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);

    pinMode(txd, ALT5);
    pinMode(rxd, ALT5);

	PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) {}
    PUT32(GPPUDCLK0,(1<<14));
    for(ra=0;ra<150;ra++) {}
    PUT32(GPPUDCLK0,0);

 PUT32(AUX_MU_CNTL_REG,3);


 // enable bcm interrupt controller
 	PUT32(IRQ_ENABLE1,1<<29);


	ra = 0;
    while(1)
    {
        while(1)
        {
            if(GET32(AUX_MU_LSR_REG)&0x20) break;
        }
        PUT32(AUX_MU_IO_REG,0x30+(ra++&7));
		log_msg("written\n");
    }
}
