#include <stddef.h>
#include <string.h>

#include "uart.h"
#include "led.h"
#include "osd_uart.h"
#include "osd_display.h"

void vTaskUART(void *params){
	const char *str = "PLAYUAV\n"; 
	size_t len = strlen(str); 
	for(int c = 0; c < len; c++){
		uart_put(str[c]); 
	}

	for(;;){
		char buf[16]; 

		int size = uart_read(buf, sizeof(buf)); 

		LEDOn(LED_BLUE); 
		
		if(size > 0)
			osd_display_process_proto_chunk(buf, size); 

		LEDOff(LED_BLUE); 
	}	
}

