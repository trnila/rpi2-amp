#pragma once

void uart_init();
void uart_print(const char *str);
char uart_read();


int irq_uart_received();