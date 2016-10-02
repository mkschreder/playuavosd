all: simulation

simulation: 
	$(CC) $(CFLAGS) -std=c99 -D_POSIX_C_SOURCE -DNO_STDOUT_REDIRECT -Wall main.c ../graphengine.c ../fonts.c ../font_outlined8x14.c ../font_outlined8x8.c -o simulation -lSDL -lutype -lm

.PHONY: simulation
