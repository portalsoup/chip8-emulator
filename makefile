all:
	gcc -o chip8 main.c graphics.c -lSDL2 `sdl-config --cflags --libs`