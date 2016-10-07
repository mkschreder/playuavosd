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
#include "../osd_display.h"

static SDL_Surface *sdl_screen = 0; 
uint8_t *draw_buffer_mask, *draw_buffer_level; 

static const struct pios_video_type_boundary pios_video_type_boundary_pal = {
	.graphics_right  = 359,         // must be: graphics_width_real - 1
	.graphics_bottom = 265,         // must be: graphics_hight_real - 1
};

const struct pios_video_type_boundary *pios_video_type_boundary_act = &pios_video_type_boundary_pal;

#define SCREEN_HEIGHT 265
#define SCREEN_WIDTH 359

static struct osd_display disp; 

static void sdl_set_pixel(int x, int y, uint32_t color){
	uint32_t offset = y * sdl_screen->w * sdl_screen->format->BytesPerPixel + x * sdl_screen->format->BytesPerPixel; 
	uint32_t *ptr = sdl_screen->pixels + offset; 
	*ptr = color; 
}

void video_get_screen_size(uint16_t *x, uint16_t *y){
	*x = SCREEN_WIDTH; 
	*y = SCREEN_HEIGHT; 
}

void _process_reading(struct xv11 *self, struct xv11_reading *r){
	char str[64] = {0}; 
	sprintf(str, "RR %d %d\n", r->angle, r->distance); 
	osd_display_process_proto_chunk(&disp, str, strlen(str)); 
}

static struct xv11_callbacks _cb = {
	.on_process_reading = _process_reading
}; 

int main(int argc, char **argv){
	struct xv11 xv11; 
	struct xv11_tty tty; 

	xv11_init(&xv11, &_cb); 
	xv11_tty_init(&tty, "/dev/ttyUSB0"); 

	osd_display_init(&disp); 

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

	while(1){
		//xv11_tty_flush(&tty); 
		char data[32]; 
		int d = xv11_tty_read(&tty, data, sizeof(data));	
		if(d > 0) xv11_process_proto_chunk(&xv11, data, d); 

		SDL_FillRect(sdl_screen, NULL, SDL_MapRGB(sdl_screen->format, 128, 128, 128)); 

		clearGraphics(); 
		osd_display_render(&disp); 
		
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

