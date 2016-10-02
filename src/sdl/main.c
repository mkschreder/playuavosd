#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <signal.h>

#include "../graphengine.h"
#include "../osdcore.h"

static SDL_Surface *sdl_screen = 0; 
uint8_t *draw_buffer_mask, *draw_buffer_level; 

static const struct pios_video_type_boundary pios_video_type_boundary_pal = {
	.graphics_right  = 359,         // must be: graphics_width_real - 1
	.graphics_bottom = 265,         // must be: graphics_hight_real - 1
};

const struct pios_video_type_boundary *pios_video_type_boundary_act = &pios_video_type_boundary_pal;

#define SCREEN_HEIGHT 265
#define SCREEN_WIDTH 359

static void sdl_set_pixel(int x, int y, uint32_t col){
	uint32_t color = SDL_MapRGB(sdl_screen->format, 255, 255, 255);  

	uint32_t offset = y * sdl_screen->w * sdl_screen->format->BytesPerPixel + x * sdl_screen->format->BytesPerPixel; 
	uint32_t *ptr = sdl_screen->pixels + offset; 
	*ptr = color; 
}

int main(int argc, char **argv){
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

	while(1){
		SDL_FillRect(sdl_screen, NULL, SDL_MapRGB(sdl_screen->format, 128, 128, 128)); 

		char tmp_str[50] = {0}; 
		sprintf(tmp_str, "Hello World!"); 
		write_string(tmp_str, 10, 10, 0, 0, TEXT_VA_TOP, TEXT_HA_LEFT, 0, 2); 

		write_line_lm(10, 15, 20, 20, 1, 1);

		write_pixel(draw_buffer_level, 20, 20, 1); 
		write_pixel(draw_buffer_level, 22, 22, 1); 
		
		for(int y = 0; y < SCREEN_HEIGHT; y++){
			for(int x = 0; x < SCREEN_WIDTH; x++){
				uint8_t *ptr = &draw_buffer_level[y * BUFFER_WIDTH + x / 8]; 
				uint8_t bit = x & 0x07; 
				if(*ptr & (0x80 >> bit)){
					sdl_set_pixel(x, y, 1); 
				}
			}
		}

		SDL_UpdateRect(sdl_screen, 0, 0, 0, 0); 
	}

	return 0; 
}

