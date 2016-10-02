/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <utype/cbuf.h>

#include "uart.h"
#include "osdconfig.h"

struct stm32_uart {
	struct cbuf rx_buf; 
	char rx_data[32]; 
}; 

struct stm32_uart uart3; 

// USART3 - MAVLINK (RX - IRQ, TX - IRQ)
void uart_init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	memset(uart3.rx_data, 0, sizeof(uart3.rx_data)); 
	cbuf_init(&uart3.rx_buf, uart3.rx_data, sizeof(uart3.rx_data)); 
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    // USART3_TX    PB10
    // USART3_RX    PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART1);
	
	//reset USART
	USART_DeInit(USART3);
	
    // RX interrupt, doesn't need high performance
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

// USART3 Rx IRQ Handler
void USART3_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	uint16_t SR = USART3->SR;

	if (SR & USART_FLAG_RXNE) {
        cbuf_put(&uart3.rx_buf, USART3->DR);
    }
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int uart_get(void){
	return cbuf_get(&uart3.rx_buf); 
}

void uart_put(u8 ch){
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET){}
	USART_SendData(USART3, (u8) ch);
}

