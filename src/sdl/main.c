#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL/SDL.h>
#include <signal.h>

#include <xv11/xv11.h>
#include <xv11/xv11_tty.h>

#include "../graphengine.h"
#include "../video.h"
#include "../osd_proto.h" 

static SDL_Surface *sdl_screen = 0; 
uint8_t *draw_buffer_mask, *draw_buffer_level; 

static const struct pios_video_type_boundary pios_video_type_boundary_pal = {
	.graphics_right  = 359,         // must be: graphics_width_real - 1
	.graphics_bottom = 265,         // must be: graphics_hight_real - 1
};

const struct pios_video_type_boundary *pios_video_type_boundary_act = &pios_video_type_boundary_pal;

#define SCREEN_HEIGHT 265
#define SCREEN_WIDTH 359

static void sdl_set_pixel(int x, int y, uint32_t color){
	uint32_t offset = y * sdl_screen->w * sdl_screen->format->BytesPerPixel + x * sdl_screen->format->BytesPerPixel; 
	uint32_t *ptr = sdl_screen->pixels + offset; 
	*ptr = color; 
}

struct osd_state {
	int16_t vbat; 
	bool 	scan; 
	uint16_t radar[360]; 
} state; 

void _process_reading(struct xv11 *self, struct xv11_reading *r){
	int i = r->angle; 
	state.radar[i % 360] = r->distance; 
}

static struct xv11_callbacks _cb = {
	.on_process_reading = _process_reading
}; 

void _proto_on_rr(struct osd_proto *self, uint16_t deg, uint16_t distance){
	state.radar[deg % 360] = distance; 
}

void _proto_on_vbat(struct osd_proto *self, uint16_t val){
	state.vbat = val; 
}

void _proto_on_scan(struct osd_proto *self, bool on){
	state.scan = on; 
}

struct osd_proto_callbacks _osd_procs = {
	.on_rr = _proto_on_rr, 
	.on_vbat = _proto_on_vbat,
	.on_scan = _proto_on_scan
}; 

int main(int argc, char **argv){
	struct osd_proto proto; 

	struct xv11 xv11; 
	struct xv11_tty tty; 

	xv11_init(&xv11, &_cb); 
	xv11_tty_init(&tty, "/dev/ttyUSB0"); 

	osd_proto_init(&proto, &_osd_procs); 

	struct sigaction action;
	sigaction(SIGINT, NULL, &action);
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		fprintf(stderr, "Could initialize sdl!\n"); 
		return -1; 
	}
	sigaction(SIGINT, &action, NULL);

	sdl_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE | SDL_ANYFORMAT); 

	if(!sdl_screen){
		fprintf(stderr, "Culd not initialize sdl surface!\n"); 
		return -1; 
	}

	draw_buffer_mask = malloc(BUFFER_HEIGHT * BUFFER_WIDTH); 
	draw_buffer_level = malloc(BUFFER_HEIGHT * BUFFER_WIDTH); 

    memset(draw_buffer_mask, 0, BUFFER_HEIGHT * BUFFER_WIDTH);
    memset(draw_buffer_level, 0, BUFFER_HEIGHT * BUFFER_WIDTH);

	printf("%d bytes-per-pixel\n", sdl_screen->format->BytesPerPixel); 
	printf("%d bits-per-pixel\n", sdl_screen->format->BitsPerPixel); 

	uint32_t white = SDL_MapRGB(sdl_screen->format, 255, 255, 255); 
	uint32_t black = SDL_MapRGB(sdl_screen->format, 0, 0, 0); 

	const char *data = "RR 10 1200\nRR 12 1300\nVBAT 1025"; 
	osd_proto_process_chunk(&proto, data, strlen(data)); 

	while(1){
		xv11_tty_flush(&tty); 
		for(int c = 0; c < 22 * 90; c++){
			int d = xv11_tty_read(&tty);	
			xv11_putc(&xv11, d); 
		}

		SDL_FillRect(sdl_screen, NULL, SDL_MapRGB(sdl_screen->format, 128, 128, 128)); 

		uint16_t half_x = SCREEN_WIDTH >> 1; 
		uint16_t half_y = SCREEN_HEIGHT >> 1; 

		clearGraphics(); 

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
		
		for(int y = 0; y < SCREEN_HEIGHT; y++){
			for(int x = 0; x < SCREEN_WIDTH; x++){
				uint8_t *lvl = &draw_buffer_level[y * BUFFER_WIDTH + x / 8]; 
				uint8_t *msk = &draw_buffer_mask[y * BUFFER_WIDTH + x / 8]; 
				uint8_t bit = x & 0x07; 
				if(*msk & (0x80 >> bit)){
					if(*lvl & (0x80 >> bit)){
						sdl_set_pixel(x, y, white); 
					} else {
						sdl_set_pixel(x, y, black); 
					}
				}
			}
		}

		SDL_UpdateRect(sdl_screen, 0, 0, 0, 0); 
	}

	return 0; 
}

