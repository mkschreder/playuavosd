#ifndef __USART2_H
#define __USART2_H

#include "board.h"

void 	uart_init(uint32_t baudRate);
int		uart_get(void); 
void 	uart_put(uint8_t ch);

#endif //__USART2_H
