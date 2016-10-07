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
#include <utype/utils.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include "osd_display.h"
#include "graphengine.h"
#include "video.h"
#include "fonts.h"
#include "osd_proto.h"

extern uint8_t *draw_buffer_level;
extern uint8_t *draw_buffer_mask;

static void _proto_on_rr(struct osd_proto *proto, uint16_t deg, uint16_t distance){
	struct osd_display *self = container_of(proto, struct osd_display, proto); 
	deg = (deg % 360) >> 1; 
	self->radar[deg] = distance; 
}

static void _proto_on_vbat(struct osd_proto *proto, uint16_t val){
	struct osd_display *self = container_of(proto, struct osd_display, proto); 
	self->vbat = val; 
}

static void _proto_on_scan(struct osd_proto *proto, bool on){
	struct osd_display *self = container_of(proto, struct osd_display, proto); 
	self->scan = on; 
}

struct osd_proto_callbacks _osd_procs = {
	.on_rr = _proto_on_rr, 
	.on_vbat = _proto_on_vbat,
	.on_scan = _proto_on_scan
}; 

void osd_display_init(struct osd_display *self){
	memset(self, 0, sizeof(struct osd_display)); 

	osd_proto_init(&self->proto, &_osd_procs); 
}

void osd_display_render(struct osd_display *self){
	uint16_t width; 
	uint16_t height; 

	video_get_screen_size(&width, &height); 

	uint16_t half_x = width >> 1; 
	uint16_t half_y = height >> 1; 

	static const float scale = 1.0f / 30.0f; 

	//char tmp_str[50] = {0}; 
	//sprintf(tmp_str, "Inkonova"); 
	//write_string(tmp_str, 295, 255, 0, 0, TEXT_VA_TOP, TEXT_HA_LEFT, 0, 2); 

	//write_line_lm(40, 123, 330, 123, 0, 0); 	
	//write_line_lm(40, 133, 330, 133, 1, 0); 	

	//write_line_lm(40, 143, 330, 143, 0, 1); 	
	//write_line_lm(40, 153, 330, 153, 1, 1); 	
	/*write_line_lm(55, 253, 55, 265, 1, 1); 	
	write_line_lm(105, 253, 105, 265, 1, 1); 	
	write_line_lm(155, 253, 155, 265, 1, 1); 	
	write_line_lm(205, 253, 205, 265, 1, 1); 	
	write_line_lm(255, 253, 255, 265, 1, 1); 	
	*/
	//write_line_lm(305, 253, 305, 265, 1, 1); 	

	write_line_outlined(half_x - 5, half_y + 5, half_x, half_y - 5, 0, 0, 0, 1);
	write_line_outlined(half_x + 5, half_y + 5, half_x, half_y - 5, 0, 0, 0, 1);
	write_line_outlined(half_x - 5, half_y + 5, half_x + 6, half_y + 5, 0, 0, 0, 1);

	write_circle_outlined(half_x, half_y, 1000.0f * scale, 10, 0, 0, 1); 

	uint16_t px = 0, py = 0; 
	bool prev_valid = false; 
	uint16_t invalid = 0; 
	for(int c = -1; c < 180; c++){            
		uint16_t d; 
		if(c == -1) d = self->radar[179]; 
		else d = self->radar[c]; 

		if(d == 0) {
			invalid++; 
			continue; 
		}
		// for invalid readings we place the dots into a circle with 3m radius
		//if(d == 0) d = 3000; 
		int16_t x = half_x + sin((c * 2 - 0) * M_PI / 180.0) * (float)d * scale;  
		int16_t y = half_y + cos((c * 2 - 0) * M_PI / 180.0) * (float)d * scale;  
			
		if(x < 0) x = 0; 
		if(y < 0) y = 0; 
		if(x >= width) x = width - 1; 
		if(y >= height) y = height - 1; 

		if(prev_valid && invalid < 25)
			write_line_outlined(px, py, x, y, 0, 0, 0, 1);

		write_circle_outlined(x, y, 1, 0, 0, 0, 1); 
		px = x; py = y; 
		prev_valid = true; 
		invalid = 0; 
	}
}

void osd_display_process_proto_chunk(struct osd_display *self, char *buf, size_t size){
	if(!size) return; 

	osd_proto_process_chunk(&self->proto, buf, size); 
}

