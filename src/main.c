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

#include <stddef.h>
#include <string.h>

#include "board.h"
#include "uart.h"
#include "osd_display.h"
#include "graphengine.h"
#include "video.h"
#include "led.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

uint64_t u64Ticks=0;        // Counts OS ticks (default = 1000Hz).
uint64_t u64IdleTicks=0;    // Value of u64IdleTicksCnt is copied once per sec.
uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
bool stackOverflow = false;

static xSemaphoreHandle osd_sem;
static struct osd_display disp; 

void vTaskDisplay(void *pvParameters){
	for(;;){
		video_wait_vsync(); 
	
		clearGraphics(); 

		//xSemaphoreTake(osd_sem, portMAX_DELAY); 
		osd_display_render(&disp); 
		//xSemaphoreGive(osd_sem); 
	}
}

void vTaskUSB(void *pvParameters){
	uint8_t c;

	GPIO_InitTypeDef  gpio;
	//This is a trick to perform a USB re-enumerate
	gpio.GPIO_Pin = GPIO_Pin_12;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &gpio);
	GPIO_SetBits(GPIOB, GPIO_Pin_12 );

	vTaskDelay(500 / portTICK_RATE_MS);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12 );

	// Initialize USB VCP. Do it ASAP
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, 
		&USR_desc, 
		&USBD_CDC_cb, 
		&USR_cb);

	for(;;){
		uint8_t ch; 

		while(VCP_get_char(&ch)){
			VCP_put_char(ch); 
			//xSemaphoreTake(osd_sem, portMAX_DELAY); 
			osd_display_process_proto_chunk(&disp, &ch, 1); 
			//xSemaphoreGive(osd_sem); 
		}
		
		// must yield because we are in a tight polling loop
		taskYIELD(); 
	}
}
void vTaskUART(void *params){
	const char *str = "PLAYUAV\n"; 
	size_t len = strlen(str); 
	for(int c = 0; c < len; c++){
		uart_put(str[c]); 
	}

	for(;;){
		char buf[64]; 
		int size; 

		while((size = uart_read(buf, sizeof(buf))) > 0){
			LEDOn(LED_BLUE); 
			
			//xSemaphoreTake(osd_sem, portMAX_DELAY); 
			osd_display_process_proto_chunk(&disp, buf, size); 
			//xSemaphoreGive(osd_sem); 

			LEDOff(LED_BLUE); 
		}
		
		// maximum uart rx rate is 14kb/s so waiting 2ms can at most mean that we get 58 bytes in the buffer. 
		// must make sure that rx buffer is bigger than that in the uart
		vTaskDelay(4 / portTICK_RATE_MS);
	}	
}

int main(void){
   	board_init();
	video_init();
	osd_display_init(&disp); 

	vSemaphoreCreateBinary(osd_sem);

	xTaskCreate( vTaskHeartBeat, (const char*)"Task Heartbeat",
		STACK_SIZE_MIN, NULL, THREAD_PRIO_LOW, NULL );

	//xTaskCreate( vTask10HZ, (const char*)"Task 10HZ",
	//	STACK_SIZE_MIN, NULL, THREAD_PRIO_NORMAL, NULL );

	xTaskCreate( vTaskDisplay, (const char*)"Task Display",
		STACK_SIZE_MIN*2, NULL, THREAD_PRIO_HIGHEST, NULL );

	xTaskCreate( vTaskUART, (const char*)"Task PROTO",
		STACK_SIZE_MIN, NULL, THREAD_PRIO_NORMAL, NULL );

	xTaskCreate( vTaskUSB, (const char*)"Task USB",
			STACK_SIZE_MIN*2, NULL, THREAD_PRIO_LOW, NULL );

	vTaskStartScheduler();
}

// This FreeRTOS callback function gets called once per tick (default = 1000Hz).
// ----------------------------------------------------------------------------
void vApplicationTickHook( void ) {
    ++u64Ticks;
}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ----------------------------------------------------------------------------
void vApplicationIdleHook( void ) {
    ++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ----------------------------------------------------------------------------
void vApplicationMallocFailedHook( void ) {
    configASSERT( 0 );  // Latch on any failure / error.
}

#define DEBUG_STACK_OVERFLOW 0
void vApplicationStackOverflowHook(uintptr_t pxTask, signed char * pcTaskName)
{
	stackOverflow = true;
#if DEBUG_STACK_OVERFLOW
	static volatile bool wait_here = true;
	while(wait_here);
	wait_here = true;
#endif
}

