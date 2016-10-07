all: simulation

simulation: 
	$(CC) $(CFLAGS) -std=gnu99 -g `sdl-config --cflags` -D_XOPEN_SOURCE=2016-D_POSIX_C_SOURCE -DNO_STDOUT_REDIRECT -Wall main.c ../osd_proto.c ../graphengine.c ../fonts.c ../font_outlined8x14.c ../osd_display.c ../font_outlined8x8.c -o simulation -lxv11 -lSDL -lutype -lm

.PHONY: simulation
