#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include <stdint.h>

#include "osd_proto.h"

static int _osd_proto_process_cmd(struct osd_proto *self) {
	char *save = NULL; 
	char *cmd = strtok_r(self->buf, " ", &save);  
	int ret = 0; 
	if(!cmd) goto done; 
	if(self->cb->on_rr && strcmp(cmd, "RR") == 0) {
		char *deg_str = strtok_r(NULL, " ", &save); 
		char *dis_str = strtok_r(NULL, " ", &save); 
		if(!deg_str || !dis_str) goto done; 
		printf("%s %s\n", deg_str, dis_str); 
		uint16_t deg = atoi(deg_str); 
		uint16_t dis = atoi(dis_str); 
		self->cb->on_rr(self, deg, dis); 
	} else if(self->cb->on_vbat && strcmp(cmd, "VBAT") == 0){
		char *v_str = strtok_r(NULL, " ", &save); 
		if(!v_str) goto done; 
		self->cb->on_vbat(self, atoi(v_str)); 
	} else if(self->cb->on_scan && strcmp(cmd, "SCAN") == 0){
		char *scan_str = strtok_r(NULL, " ", &save); 
		if(!scan_str) goto done; 
		if(strcasecmp(scan_str, "on") == 0){
			self->cb->on_scan(self, true); 
		} else if(strcasecmp(scan_str, "off") == 0){
			self->cb->on_scan(self, false); 
		}
	}
	ret = 1; 
done: 
	memset(self->buf, 0, sizeof(self->buf)); 
	self->buf_pos = 0; 
	return ret; 
}

int osd_proto_process_byte(struct osd_proto *self, char ch){
	int ret = 0; 
	switch(ch){
		case '\n': {
			// process command if there is something in our buffer
			if(self->buf_pos != 0){
				ret += _osd_proto_process_cmd(self); 
				break; 
			}
			// otherwise newlines are discarded
		} break; 
		default: { 
			self->buf[self->buf_pos++] = ch; 
			if(self->buf_pos == sizeof(self->buf)){
				// reset the buffer since this should never happen
				self->buf_pos = 0; 
				memset(self->buf, 0, sizeof(self->buf));
			}
		} break; 
	}
	return ret; 
}

void osd_proto_init(struct osd_proto *self, struct osd_proto_callbacks *cb){
	self->cb = cb; 
	self->buf_pos = 0; 
	memset(self->buf, 0, sizeof(self->buf)); 
}

int osd_proto_process_chunk(struct osd_proto *self, const char *data, size_t size){
	int ret = 0; 
	for(size_t i = 0; i < size; i++){
		ret += osd_proto_process_byte(self, *data); 
		data++; 
	}
	return ret; 
}

