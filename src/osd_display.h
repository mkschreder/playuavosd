#pragma once

#include <stddef.h>
#include "osd_proto.h"

struct osd_display {
	struct osd_proto proto; 
	int16_t vbat; 
	bool 	scan; 
	int16_t radar[180]; 
};

void osd_display_init(struct osd_display *self); 

// draws the display to the graphics buffer
void osd_display_render(struct osd_display *self); 

// display control protocol parser
void osd_display_process_proto_chunk(struct osd_display *self, char *data, size_t size); 
