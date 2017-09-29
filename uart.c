#include "api.h"
#include "const.h"
#include "FreeRTOS.h"
#include "queue.h"

#define WORD_8BIT 3
#define WORD_7BIT 2

#define IER_INT_NO 0
#define IER_INT_TXRD 1
#define IER_INT_RXRD 2

#define CNTL_ENABLE_RX BIT(0)
#define CNTL_ENABLE_TX BIT(1)

#define BUFFER_SIZE 32


QueueHandle_t rxQueue;

int irq_uart_received() {
    int shouldSwitch = 0;

    int c = REG(AUX_MU_IO_REG);
    xQueueSendToBackFromISR(rxQueue, &c, &shouldSwitch);

    return shouldSwitch;
}

char uart_read() {
    char val;
    xQueueReceive(rxQueue, &val, portMAX_DELAY);
    return val;
}

void uart_print(const char *str) {
    while(*str) {
        while(!(REG(AUX_MU_LSR_REG) & 0x20)) {}
        REG(AUX_MU_IO_REG) = *str;
        str++;
    }
}

void uart_init() {
	const int txd = 14;
	const int rxd = 15;

    pinMode(txd, ALT5);
    pinMode(rxd, ALT5);

    // enable mini uart
	REG(AUX_ENABLES) |= 1;

    // disable transmit and receive before we configure it
    REG(AUX_MU_CNTL_REG) = 0;

    REG(AUX_MU_LCR_REG) = WORD_8BIT;
    REG(AUX_MU_MCR_REG) = 0;
    REG(AUX_MU_IER_REG) = IER_INT_RXRD; // enable interrupts
    REG(AUX_MU_IIR_REG) = 0;
    REG(AUX_MU_BAUD_REG) = 270; //TODO: configurable baud rate

    REG(AUX_MU_CNTL_REG) = CNTL_ENABLE_RX | CNTL_ENABLE_TX;

    // enable bcm interrupt controller
 	REG(IRQ_ENABLE1) = 1 << 29;

    rxQueue = xQueueCreate(BUFFER_SIZE, sizeof(char));
    if(!rxQueue) {
        panic("Could not create rx queue");
    }
}
