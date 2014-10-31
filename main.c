#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//#include "chip8.h"
#include "graphics.h"

int main(int argc, char** argv) {

	printf("Setting up SDL...\n");
	setup_SDL();

	char gfx[64*32];

	int _count;
	int count;
	char temp = 0;
	for (_count = 0; _count < 10; _count++) {
		srand(time(NULL));
		for (count = 0; count < 64*32; count++) {
			if (rand() % 2 == 0) {
				temp = 0x71;
			} else {
				temp = 0x70;
			}
			gfx[count] = temp;
		}

		draw_SDL(&gfx);
		sleep(1);
	}



	//printf("Setting up input...\n");
	//setup_input();

	//initialize();
	//load_game("PONG");

/*
	while (1) {
		emulateCycle();

		if (&drawFlag) {
			drawGraphics();
		}

		set_keys();
	}
*/
	return 0;
}

/*
void emulate_cycle() {
	//fetch opcode
	decode_next();

	//update timers
	if (delay_timer > 0) {
		--delay_timer;
	}

	if (sound_timer > 0) {
		if (sound_timer == 1) {
			printf("BEEP\n");
		}
		--sound_timer;
	}

}
*/