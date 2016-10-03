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
/*
 * With Grateful Acknowledgements to the projects:
 * Tau Labs - Brain FPV Flight Controller(https://github.com/BrainFPV/TauLabs)
 */
#include "osdproc.h"
#include "graphengine.h"
#include "led.h"
#include "video.h"
#include "osdvar.h"
#include "fonts.h"
#include "osdconfig.h"
#include "osd_proto.h"
#include "uart.h"

extern uint8_t *draw_buffer_level;
extern uint8_t *draw_buffer_mask;

extern xSemaphoreHandle onScreenDisplaySemaphore;

struct osd_state {
	int16_t vbat; 
	bool 	scan; 
	uint16_t radar[360]; 
} state; 

static void _proto_on_rr(struct osd_proto *self, uint16_t deg, uint16_t distance){
	state.radar[deg % 360] = distance; 
}

static void _proto_on_vbat(struct osd_proto *self, uint16_t val){
	state.vbat = val; 
}

static void _proto_on_scan(struct osd_proto *self, bool on){
	state.scan = on; 
}

struct osd_proto_callbacks _osd_procs = {
	.on_rr = _proto_on_rr, 
	.on_vbat = _proto_on_vbat,
	.on_scan = _proto_on_scan
}; 

static void _render_screen(void){
	// TODO: remove and replace with the proper pal size
	const uint16_t SCREEN_WIDTH = 359; 
	const uint16_t SCREEN_HEIGHT = 265; 

	uint16_t half_x = SCREEN_WIDTH >> 1; 
	uint16_t half_y = SCREEN_HEIGHT >> 1; 

	static const float scale = 20.0f; 

	char tmp_str[50] = {0}; 
	sprintf(tmp_str, "Inkonova"); 
	write_string(tmp_str, 295, 255, 0, 0, TEXT_VA_TOP, TEXT_HA_LEFT, 0, 2); 

	write_line_lm(0, 253, 359, 253, 1, 1); 	
	/*write_line_lm(55, 253, 55, 265, 1, 1); 	
	write_line_lm(105, 253, 105, 265, 1, 1); 	
	write_line_lm(155, 253, 155, 265, 1, 1); 	
	write_line_lm(205, 253, 205, 265, 1, 1); 	
	write_line_lm(255, 253, 255, 265, 1, 1); 	
	*/
	//write_line_lm(305, 253, 305, 265, 1, 1); 	

	write_line_lm(half_x - 5, half_y + 5, half_x, half_y - 5, 1, 1);
	write_line_lm(half_x + 5, half_y + 5, half_x, half_y - 5, 1, 1);
	write_line_lm(half_x - 5, half_y + 5, half_x + 6, half_y + 5, 1, 1);
	
	write_circle(draw_buffer_level, half_x, half_y, 500.0f / scale, 0, 1); 
	write_circle(draw_buffer_mask, half_x, half_y, 500.0f / scale, 0, 1); 

	write_circle(draw_buffer_level, half_x, half_y, 1000.0f / scale, 10, 1); 
	write_circle(draw_buffer_mask, half_x, half_y, 1000.0f / scale, 10, 1); 

	for(int c = 1; c < 360; c++){            
		uint16_t x = half_x + sin((c - 0) * M_PI / 180.0) * (float)state.radar[c] / scale;  
		uint16_t y = half_y + cos((c - 0) * M_PI / 180.0) * (float)state.radar[c] / scale;  
		write_pixel(draw_buffer_level, x, y, 1);
		write_pixel(draw_buffer_mask, x, y, 1);
	}
}

void vTaskOSD(void *pvParameters){
	video_set_x_offset(osd_offset_X);
	video_set_y_offset(osd_offset_Y);

	video_init();

	struct osd_proto proto; 

	osd_proto_init(&proto, &_osd_procs); 

	for(;;)
	{
		xSemaphoreTake(onScreenDisplaySemaphore, portMAX_DELAY);
		
		for(int c = 0; c < 16; c++){
			int16_t ch = uart_get(); 
			if(ch < 0) break; 
			//uart_put(ch); 
			osd_proto_process_byte(&proto, ch); 
		}

		clearGraphics();

		_render_screen();
	}
}

