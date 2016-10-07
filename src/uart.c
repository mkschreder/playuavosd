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

#include <errno.h>
#include <utype/cbuf.h>

#include "uart.h"

struct stm32_uart {
	struct cbuf rx_buf; 
	char rx_data[128]; 

	xSemaphoreHandle rx_sem;
}; 

static struct stm32_uart uart3; 

// USART3 - MAVLINK (RX - IRQ, TX - IRQ)
void uart_init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	memset(uart3.rx_data, 0, sizeof(uart3.rx_data)); 
	cbuf_init(&uart3.rx_buf, uart3.rx_data, sizeof(uart3.rx_data)); 

	vSemaphoreCreateBinary(uart3.rx_sem);
	
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

	// init the uart led (on playuav we use the "blue" led
	/*
	GPIO_InitTypeDef  gpio_led;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	gpio_led.GPIO_Pin = GPIO_Pin_5;
	gpio_led.GPIO_Mode = GPIO_Mode_OUT;
	gpio_led.GPIO_OType = GPIO_OType_PP;
	gpio_led.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_led.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_led);
	*/
}
/*
static void _led_on(void){
	GPIOC->BSRRH = GPIO_Pin_5;
}

static void _led_off(void){
	GPIOC->BSRRL = GPIO_Pin_5;
}
*/
// USART3 Rx IRQ Handler
void USART3_IRQHandler(void){
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		cbuf_put(&uart3.rx_buf, USART_ReceiveData(USART3));

		xSemaphoreGiveFromISR(uart3.rx_sem, &xHigherPriorityTaskWoken);
    }
	
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int uart_read(char *data, size_t size){
	// wait for at least one data byte to be available
	xSemaphoreTake(uart3.rx_sem, portMAX_DELAY);

	if(cbuf_empty(&uart3.rx_buf)) return -EAGAIN; 

	size_t c = 0; 
	for(c = 0; c < size; c++){
		int16_t d = cbuf_get(&uart3.rx_buf); 
		if((d & 0xff00) != 0) break; 

		*data = d & 0xff; 
		data++; 
	}
	return c; 
}

void uart_put(u8 ch){
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}
	USART_SendData(USART3, (u8) ch);
}

