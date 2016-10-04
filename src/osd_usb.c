#include "board.h"
#include "osd_display.h"
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

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
			osd_display_process_proto_chunk(&ch, 1); 
		}
		
		// must yield because we are in a tight polling loop
		taskYIELD(); 
	}
}
