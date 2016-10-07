#pragma once

#include <stdbool.h>
#include <stdint.h>

struct osd_proto; 

struct osd_proto_callbacks {
	void (*on_rr)(struct osd_proto *self, uint16_t deg, uint16_t distance); 
	void (*on_vbat)(struct osd_proto *self, uint16_t val); 
	void (*on_scan)(struct osd_proto *self, bool on); 
}; 

struct osd_proto {
	struct osd_proto_callbacks *cb;
	char buf[16]; 
	uint8_t buf_pos; 
}; 

void osd_proto_init(struct osd_proto *self, struct osd_proto_callbacks *cb); 
int osd_proto_process_byte(struct osd_proto *self, char ch); 
int osd_proto_process_chunk(struct osd_proto *self, const char *data, size_t size); 
