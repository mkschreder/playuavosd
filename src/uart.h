#ifndef __USART2_H
#define __USART2_H

#include "board.h"

void 	uart_init(uint32_t baudRate);
int 	uart_read(char *buf, size_t max_size); 
void 	uart_put(uint8_t ch);

#endif //__USART2_H
