#pragma once

#include <stddef.h>

void vTaskDisplay(void *pvParameters);
void osd_display_process_proto_chunk(char *data, size_t size); 
