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

#include "board.h"
#include "osd_display.h"
#include "osd_usb.h"
#include "osd_uart.h"

uint64_t u64Ticks=0;        // Counts OS ticks (default = 1000Hz).
uint64_t u64IdleTicks=0;    // Value of u64IdleTicksCnt is copied once per sec.
uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
bool stackOverflow = false;

int main(void){
   	board_init();

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

