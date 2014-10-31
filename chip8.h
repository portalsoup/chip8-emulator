void initialize();

void loadGame(char* name);

void decode();

void setupInput();

void setKeys();

void cls();

void ret();

void jump(short opcode);

void jumpToSub(short opcode);

void skipEqualsVXRR(short opcode);

void skipNotEqualsVXRR(short opcode);

void skipEqualsVXVY(short opcode);

void movRRToVX(short opcode);

void addRRVX(short opcode);

void movVXVY(short opcode);

void bitwiseORVXVY(short opcode);

void bitwiseANDVXVY(short opcode);

void bitwiseXORVXVY(short opcode);

void addWitHCarry(short opcode);

void shiftRight(short opcode);

void subtractVXVY(short opcode);

void shiftLeft(short opcode);

void skipNotEqualsVXVY(short opcode);

void loadI(short opcode);

void jumpAddV0(short opcode);

void randomVX(short opcode);

void drawSprite(short opcode);

void skipIfKeyPressed(short opcode);

void skipifKeyNotPressed(short opcode);

void storeDelayTimerInVX(short opcode);

void waitForKey(short opcode);

void setDelayTimer(short opcode);

void setSoundTimer(short opcode);

void addIVX(short opcode);

void getLocationOfSpriteVX(short opcode);

void storeDecValueVX(short opcode);

void loadIVX(short opcode);

void loadVXI(short opcode);