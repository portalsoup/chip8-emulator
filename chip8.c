#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

//headers

/* - - - - - - - - - - CPU - - - - - - - - - - - - - - */
unsigned short opcode; //One opcode is divided across two memory cells.

unsigned char memory[4096]; //Memory

unsigned char V[16]; //Registers 1-16

unsigned char VF;

unsigned short I; //Index register

unsigned short pc; //Program counter

/* System memory map
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/

unsigned char gfx[64*32]; //The screen
unsigned int drawFlag; //boolean

unsigned char delay_timer; //A delay timer
unsigned char sound_timer; //A sound timer, buzzes when decremented to 1.

unsigned short stack[16]; //The stack
unsigned short sp; 

unsigned char key[16];

/* The font.  Taking the sets of 5 hex values, lay them out in order of bits in a 4x5 grid
	
	For example: 0                  For example: 5
			0xF0 = 1111 0000    ****  |  0xF0 = 1111 0000    ****
			0x90 = 1001 0000    *  *  |  0x80 = 1000 0000    *   
			0x90 = 1001 0000    *  *  |  0xF0 = 1111 0000    ****
			0x90 = 1001 0000    *  *  |  0x10 = 0001 0000       *
			0xF0 = 1111 0000    ****  |  0xF0 = 1111 0000    ****
*/
unsigned char chip8_fontset[80] = 
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void initialize() {
	//Initialize registers and memory

	pc     = 0x200; //program counter starts here
	opcode = 0;
	I      = 0;
	sp     = 0;

	//clear display
	//clear stack
	//clear registers
	//clear memory

	//load fontset
	int counter = 0;
	while (counter < 80) {
		memory[counter] = chip8_fontset[counter];
		counter++;
	}
}




void loadGame(char* name) {
	unsigned int maxGameSize = 3584;
	char gameBuffer[maxGameSize];

	FILE* game = fopen(name, "rb");

	if (game == NULL) {
		perror("Failed to open game.");
	} else {
		printf("Game loaded! \n");
	}

    fread(gameBuffer, sizeof(char), maxGameSize, game);
    fclose(game);

    //Now load the game into the memory.
	int counter = 0;
	while (counter < maxGameSize) {
			memory[counter+512] = gameBuffer[counter];
		counter++;
	}

}


/*
void loadGame(char* name) {
	//Use fopen to read the program into memory, this loop will use the stream

	FILE* game = fopen(name, "r"); //Open the game file as a stream.
	unsigned int maxGameSize = 3584; //The max game size available 0x200 - 0xFFF
	char gameBuffer[maxGameSize]; //The buffer that the game will be temporarily loaded into.

	if (game == NULL) {
		perror("Failed to open game.");
	} else {
		while (!feof(game)) {
			if (fgets(gameBuffer, maxGameSize, game) == NULL) { //load the file into the buffer.
				break; //Reached the EOF
			}
		}

		//Now load the game into the memory.
		int counter = 0;
		while (counter < maxGameSize) {
			memory[counter+512] = gameBuffer[counter];
			counter++;
		}

	}

	fclose(game);
}
*/

void decode_next() {
	decode(memory[pc] << 8 | memory[pc+1]);
}

void decode(short opcode) {
	short mask = 0xF000 & opcode;
	//0x00xx
	if ((0xFF00 & opcode) == 0x0000) {

		//0xxxEx
		if ((0x00F0 & opcode) == 0x00E0) {
			if ((0x000F & opcode) == 0x0000) {
				cls();
			} else if (( 0x000F & opcode) == 0x000E) {
				ret();
			}
		}
	} else if (mask == 0x1000) {
		jump(opcode);
	} else if (mask == 0x2000) {
		jumpToSub(opcode);
	} else if (mask == 0x3000) {
		skipEqualsVXRR(opcode);
	} else if (mask == 0x4000) {
		skipNotEqualsVXRR(opcode);
	} else if (mask == 0x5000) {
		skipEqualsVXVY(opcode);
	} else if (mask == 0x6000) {
		moveRRToVX(opcode);
	} else if (mask == 0x7000) {
		addRRVX(opcode);
	} else if (mask == 0x8000) {

		short eightmask = 0x000F & opcode;
		if (eightmask == 0x0000) {
			movVXVY(opcode);
		} else if (eightmask == 0x0001) {
			bitwiseORVXVY(opcode);
		} else if (eightmask == 0x0002) {
			bitwiseANDVXVY(opcode);
		} else if (eightmask == 0x0003) {
			bitwiseXORVXVY(opcode);
		} else if (eightmask == 0x0004) {
			addWithCarry(opcode);
		} else if (eightmask == 0x0006) {
			shiftRight(opcode);
		} else if (eightmask == 0x0007) {
			subtractVXVY(opcode);
		} else if (eightmask == 0x000E) {
			shiftLeft(opcode);
		}
	} else if (mask == 0x9000) {
		skipNotEqualsVXVY(opcode);
	} else if (mask == 0xA000) {
		loadI(opcode);
	} else if (mask == 0xB000) {
		jumpAddV0(opcode);
	} else if (mask == 0xC000) {
		randomVX(opcode);
	} else if (mask == 0xD000) {
		drawSprite(opcode);
	} else if (mask == 0xE000) {

		if ((0x00F0 & opcode) == 0x0090) {
			skipIfKeyPressed(opcode);
		} else if ((0x00F0 & opcode) == 0x00A0) {
			skipIfKeyNotPressed(opcode);
		}

	} else if (mask == 0xF000) {

		short fMask = 0x00FF & opcode;

		if (fMask == 0x0007) {
			storeDelayTimerInVX(opcode);
		} else if (fMask == 0x000A) {
			waitForKey(opcode);
		} else if (fMask == 0x0015) {
			setDelayTimer(opcode);
		} else if (fMask == 0x0018) {
			setSoundTimer(opcode);
		} else if (fMask == 0x001E) {
			addIVX(opcode);
		} else if (fMask == 0x0029) {
			getLocationOfSpriteVX(opcode);
		} else if (fMask == 0x0033) {
			storeDecValueVX(opcode);
		} else if (fMask == 0x0055) {
			loadIVX(opcode);
		} else if (fMask == 0x0065) {
			loadVXI(opcode);
		}
	}
}


void setupInput() {

}

void setKeys() {

}


/* - - - - - - - - - - - - Op codes - - - - - - - - - - */

/*
 * 0x00E0
 *
 * Clears the screen
 */
void cls() {

}

/*
 * 0x00EE
 * 
 * Return from a subroutine
 */
void ret() {

 }

 /*
  * 0x1NNN
  *
  * Jump to address NNN
  */
void jump(short opcode) {

}

/*
 * 0x2NNN
 *
 * Jump to subroutine at address NNN
 */
void jumpToSub(short opcode) {

}

/*
 * 0x3XRR
 *
 * Skip next instruction if register VX == constant RR
 */
void skipEqualsVXRR(short opcode) {
	short rr = 0x00FF & opcode;
	short vxmask = (0x0F00 & opcode) >> 8;
	if (rr == V[vxmask]) {
		pc += 2;
	}
}

/*
 * 0x4XRR
 *
 * Skip next instruction if register VX != constant RR
 */
void skipNotEqualsVXRR(short opcode) {
	short rr = 0x00FF & opcode;
	short vx = (0x0F00 & opcode) >> 8;

	if (rr != V[vx]) {
		pc += 2;
	}
}

/*
 * 0x5XY0
 *
 * Skip next instruction if VX == VR.
 */
void skipEqualsVXVY(short opcode) {
	short vx = (0x0F00 & opcode) >> 8;
	short vy = (0x00F0 & opcode) >> 4;

	if (vx == vy) {
		pc += 2;
	}
}

/*
 * 0x6XRR
 *
 * Move constant RR to register VX
 */
void moveRRToVX(short opcode) {
	short rr = 0x00FF & opcode;
	short vx = (0x0F00 & opcode) >> 8;

	V[vx] = rr;
}

/*
 * 0x7XRR
 *
 * Add constant RR to register VX
 */
void addRRVX(short opcode) {
	short rr = 0x00FF & opcode;
	short vx = (0x0F00 & opcode) >> 8;

	V[vx] += rr;
}

/*
 * 0x8XY0
 *
 * Move register VY into VX
 */
void movVXVY(short opcode) {
	short vx = (0x0F00 & opcode) >> 8;
	short vy = (0x00F0 & opcode) >> 4;

	V[vx] = V[vy];
}

/*
 * 0x8XY1
 *
 * bitwise OR VX | VY and store result in VX
 */
void bitwiseORVXVY(short opcode) {
	short vx = (0x0F00 & opcode) >> 8;
	short vy = (0x00F0 & opcode) >> 4;

	V[vx] = V[vx] | V[vy];
}

/*
 * 0x8XY2
 *
 * bitwise AND VX & VY and store result in VX
 */
void bitwiseANDVXVY(short opcode) {
 	short vx = (0x0f00 & opcode) >> 8;
 	short vy = (0x00f0 & opcode) >> 4;

 	V[vx] = V[vx] & V[vy];
 }

/*
 * 0x8XY3
 *
 * bitwise XOR VX, VY and store result in VX
 */
void bitwiseXORVXVY(short opcode) {

}

/*
 * 0x8XY4
 *
 * Add register VY and VX and store result in VX.
 * Carry stored in VF
 */
void addWithCarry(short opcode) {

}

/*
 * 0x8X06
 *
 * Shift register VX right.  bit 0 goes into VF.
 */
void shiftRight(short opcode) {

}

/*
 * 0x8XY7
 *
 * Subtract VX from VY and store result in VX
 */
void subtractVXVY(short opcode) {

}

/*
 * 0x8X0E
 *
 * Shift VX left, bit 7 stored in VF
 */
void shiftLeft(short opcode) {

}

/*
 * 0x9XY0
 *
 * Skip next instruction if VX != VY
 */
void skipNotEqualsVXVY(short opcode) {

}

/*
 * 0xANNN
 *
 * Load index register (I) with constant NNN
 */
void loadI(short opcode) {

}

/*
 * 0xBNNN
 *
 * Jump to address NNN + register V0
 */
void jumpAddV0(short opcode) {

}

/*
 * 0xCXKK
 *
 * Register VX = random number AND KK
 */
void randomVX(short opcode) {

}

/*
 * 0xDXYN
 *
 * Draw sprite at screen location (Register VX, VY)
 * height N
 */
void drawSprite(short opcode) {

}

/*
 * 0xEX9E
 * 
 * Skip next instruction if key with value VX is pressed
 */
void skipIfKeyPressed(short opcode) {

}

/*
 * 0xEXA1
 *
 * Skip next instruction if key with value VX is not pressed
 */
void skipIfKeyNotPressed(short opcode) {

}

/*
 * 0xFX07
 *
 * Stores the delay timer value in VX
 */
void storeDelayTimerInVX(short opcode) {

}

/*
 * 0xFX0A
 *
 * Wait for key press, store the value in VX
 */
void waitForKey(short opcode) {

}

/*
 * 0xFX15
 *
 * Delay timer is set equal to the value of VX
 */
void setDelayTimer(short opcode) {

}

/*
 * 0xFX18
 *
 * Sound timer is set equal to the value of VX
 */
void setSoundTimer(short opcode) {

}

/*
 * 0xFX1E
 *
 * Add I and VX and store the result in I
 */
void addIVX(short opcode) {

}

/*
 * 0xFX29
 *
 * Set I to the location for the hex sprite corresponding
 * to the value of VX.
 */
void getLocationOfSpriteVX(short opcode) {

}

/*
 * 0xFX33
 *
 * The interpreter takes the decimal value of VX and places
 * the hundreds digit in I, the tens digit in I+1 and the
 * ones digit in I+2
 */
void storeDecValueVX(short opcode) {

}

/*
 * 0xFX55
 *
 * Copies the values of V0 to VX and stores the results starting
 * at I
 */
void loadIVX(short opcode) {

}

/*
 * 0xFX65
 *
 * The interpreter reads values from memory starting at I
 * into V0 through VX
 */
void loadVXI(short opcode) {

}
