#pragma once

void uart_init();
void uart_print(const char *str);


void irq_uart_received();