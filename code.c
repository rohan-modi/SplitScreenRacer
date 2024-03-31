#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
	
volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];
	
unsigned char key1 = 0;
unsigned char key2 = 0;
unsigned char key3 = 0;

bool wPressed = false;
bool aPressed = false;
bool sPressed = false;
bool dPressed = false;
bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;

int xSize = 320-1;
int ySize = 240-1;
int xMin = 0;
int yMin = 0;
int mapEndX = 850;

int gravity = 1;
int playerSize = 5;
int platformSize = 5;

const short int WHITE = 0xFFFF;
const short int BLACK = 0x0000;
const short int RED = 0xF800;
const short int BLUE = 0x001F;
const short int GREEN = 0x07E0;

struct platform {
	int startX;
	int startY;
	int width;
	int height;
	short int colour;
	int prevStartX;
	int prevStartY;
	int prevWidth;
	int prevHeight;
};

struct platformTemplate {
	int startX;
	int startY;
	int width;
	int height;
	short int colour;
};

struct Player {
	int x;
	int y;
	int pastX;
	int pastY;
	int speed;
	int jumpSpeed;
	int yVelocity;
    bool* upControl;
    bool* downControl;
    bool* leftControl;
    bool* rightControl;
	short int colour;
	int score;
	int prevScore;
};

const int zero[]={0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x01,0x01};
const int one[]={0x00,0x01,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x01,0x01,0x01};
const int two[]={0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01};
const int three[]={0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01};
const int four[]={0x01,0x00,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x01};
const int five[]={0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01};
const int six[]={0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01};
const int seven[]={0x01,0x01,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01};
const int eight[]={0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01};
const int nine[]={0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01};

void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void swap(int* num1, int* num2);
void checkKey(bool* keyBool, unsigned char code, bool extendedChar);
void updateKeys();
void setSolidScreen(short int colour);
void drawRectangle(int xStart, int yStart, int width, int height, short int colour);
void drawPlatforms(struct platform platforms[], int numberOfPlatforms);
void erasePlatforms(struct platform platforms[], int numberOfPlatforms);
void movePlayer(struct Player* player, int gravity, struct platform platforms[], int numberOfPlatforms, struct platform platformLocations[]);
void drawDigit(int number[], int startX, int startY, short int colour);
void drawScore(int y, int score, short int colour, int digits[10][15]);

int main(void) {
	// Setup
	volatile int* pixel_ctrl_ptr = (int*)0xFF203020;
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
    wait_for_vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
    setSolidScreen(BLACK);
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
		
	// Player setup
    struct Player player1 = {xSize/2, ySize/2+15, xSize/2, ySize/2+15, 3, -10, 0, &wPressed, &sPressed, &aPressed, &dPressed, RED, 0, 0};
    struct Player player2 = {xSize/2, 6, xSize/2, 6, 3, -10, 0, &upPressed, &downPressed, &leftPressed, &rightPressed, WHITE, 0, 0};
	
	int borderStartY = ySize/2-5;
	int borderEndY = ySize/2+5;
	int topBottomYDifference = (ySize-platformSize) - (borderStartY);

	int digits[10][15];
	for (int i = 0; i < 15; i++) {
		digits[0][i] = zero[i];
		digits[1][i] = one[i];
		digits[2][i] = two[i];
		digits[3][i] = three[i];
		digits[4][i] = four[i];
		digits[5][i] = five[i];
		digits[6][i] = six[i];
		digits[7][i] = seven[i];
		digits[8][i] = eight[i];
		digits[9][i] = nine[i];
	}
	
	// Player 1 front platforms
	struct platform platformF1_1 = {xMin, borderStartY, xSize, platformSize, BLUE, xMin, yMin, xSize, platformSize};
	struct platform platformF1_2 = {xMin, (ySize/2-platformSize), xSize, platformSize*2, BLUE, xMin, (ySize/2-platformSize), xSize, platformSize*2};
	struct platform platformF1_3 = {xMin, ySize-platformSize, xSize, platformSize, BLUE, xMin, ySize-platformSize, xSize, platformSize};
	struct platform platformF1_4 = {xMin, borderEndY+1, platformSize, ySize/2-platformSize*2, BLUE, xMin, borderEndY+1, platformSize, ySize/2-platformSize*2};
	struct platform platformF1_5 = {mapEndX, borderEndY+1, platformSize, ySize/2-platformSize*2, BLUE, mapEndX, borderEndY+1, platformSize, ySize/2-platformSize*2};
	// Player 1 back platforms
	struct platform platformB1_1 = {xMin, borderStartY, xSize, platformSize, BLUE, xMin, yMin, xSize, platformSize};
	struct platform platformB1_2 = {xMin, (ySize/2-platformSize), xSize, platformSize*2, BLUE, xMin, (ySize/2-platformSize), xSize, platformSize*2};
	struct platform platformB1_3 = {xMin, ySize-platformSize, xSize, platformSize, BLUE, xMin, ySize-platformSize, xSize, platformSize};
	struct platform platformB1_4 = {xMin, borderEndY+1, platformSize, ySize/2-platformSize*2, BLUE, xMin, borderEndY+1, platformSize, ySize/2-platformSize*2};
	struct platform platformB1_5 = {mapEndX, borderEndY+1, platformSize, ySize/2-platformSize*2, BLUE, mapEndX, borderEndY+1, platformSize, ySize/2-platformSize*2};
	// Player 2 front platforms
	struct platform platformF2_1 = {xMin, yMin, xSize, platformSize, BLUE, xMin, yMin, xSize, platformSize};
	struct platform platformF2_2 = {xMin, (ySize/2-platformSize), xSize, platformSize*2, BLUE, xMin, (ySize/2-platformSize), xSize, platformSize*2};
	struct platform platformF2_3 = {xMin, ySize-platformSize, xSize, platformSize, BLUE, xMin, ySize-platformSize, xSize, platformSize};
	struct platform platformF2_4 = {xMin, yMin+platformSize, platformSize, ySize/2-platformSize*2, BLUE, xMin, yMin+platformSize, platformSize, ySize/2-platformSize*2};
	struct platform platformF2_5 = {mapEndX, yMin+platformSize, platformSize, ySize/2-platformSize*2, BLUE, mapEndX, yMin+platformSize, platformSize, ySize/2-platformSize*2};
	// Player 2 back platforms
	struct platform platformB2_1 = {xMin, yMin, xSize, platformSize, BLUE, xMin, yMin, xSize, platformSize};
	struct platform platformB2_2 = {xMin, (ySize/2-platformSize), xSize, platformSize*2, BLUE, xMin, (ySize/2-platformSize), xSize, platformSize*2};
	struct platform platformB2_3 = {xMin, ySize-platformSize, xSize, platformSize, BLUE, xMin, ySize-platformSize, xSize, platformSize};
	struct platform platformB2_4 = {xMin, yMin+platformSize, platformSize, ySize/2-platformSize*2, BLUE, xMin, yMin+platformSize, platformSize, ySize/2-platformSize*2};
	struct platform platformB2_5 = {mapEndX, yMin+platformSize, platformSize, ySize/2-platformSize*2, BLUE, mapEndX, yMin+platformSize, platformSize, ySize/2-platformSize*2};
		
	// Jumpers
	struct platform platformF1_6;
	struct platform platformF1_7;
	struct platform platformF1_8;
	struct platform platformF1_9;
	struct platform platformF1_10;
	struct platform platformF1_11;
	struct platform platformF1_12;
	struct platform platformF1_13;
	struct platform platformF1_14;
	// Jumpers
	struct platform platformB1_6;
	struct platform platformB1_7;
	struct platform platformB1_8;
	struct platform platformB1_9;
	struct platform platformB1_10;
	struct platform platformB1_11;
	struct platform platformB1_12;
	struct platform platformB1_13;
	struct platform platformB1_14;
	// Jumpers
	struct platform platformF2_6;
	struct platform platformF2_7;
	struct platform platformF2_8;
	struct platform platformF2_9;
	struct platform platformF2_10;
	struct platform platformF2_11;
	struct platform platformF2_12;
	struct platform platformF2_13;
	struct platform platformF2_14;
	// Jumpers
	struct platform platformB2_6;
	struct platform platformB2_7;
	struct platform platformB2_8;
	struct platform platformB2_9;
	struct platform platformB2_10;
	struct platform platformB2_11;
	struct platform platformB2_12;
	struct platform platformB2_13;
	struct platform platformB2_14;
		
	struct platformTemplate platform1 = {100, borderEndY+75, 50, platformSize, GREEN};
	struct platformTemplate platform2 = {175, borderEndY+50, 50, platformSize, GREEN};
	struct platformTemplate platform3 = {300, borderEndY+75, 50, platformSize, GREEN};
	struct platformTemplate platform4 = {385, ySize-50-platformSize, platformSize, 50, GREEN};
	struct platformTemplate platform5 = {425, borderEndY+75, 75, platformSize, GREEN};
	struct platformTemplate platform6 = {525, borderEndY+50, 75, platformSize, GREEN};
	struct platformTemplate platform7 = {625, borderEndY+25, 75, platformSize, GREEN};
	struct platformTemplate platform8 = {725, borderEndY+50, 75, platformSize, GREEN};
	struct platformTemplate platform9 = {800, borderEndY+50, platformSize, ySize-borderEndY-platformSize-50, GREEN};
	struct platformTemplate platformSetup[] = {platform1, platform2, platform3, platform4, platform5, platform6, platform7, platform8, platform9};
	
	struct platform platforms1[] = {platformF1_1, platformF1_2, platformF1_3, platformF1_4, platformF1_5, platformF1_6, platformF1_7, platformF1_8, platformF1_9, platformF1_10, platformF1_11, platformF1_12, platformF1_13, platformF1_14};
	struct platform platformLocations1[] = {platformB1_1, platformB1_2, platformB1_3, platformB1_4, platformB1_5, platformB1_6, platformB1_7, platformB1_8, platformB1_9, platformB1_10, platformB1_11, platformB1_12, platformB1_13, platformB1_14};
	
	struct platform platforms2[] = {platformF2_1, platformF2_2, platformF2_3, platformF2_4, platformF2_5, platformF2_6, platformF2_7, platformF2_8, platformF2_9, platformF2_10, platformF2_11, platformF2_12, platformF2_13, platformF2_14};
	struct platform platformLocations2[] = {platformB2_1, platformB2_2, platformB2_3, platformB2_4, platformB2_5, platformB2_6, platformB2_7, platformB2_8, platformB2_9, platformB2_10, platformB2_11, platformB2_12, platformB2_13, platformB2_14};
		
	int numberOfPlatforms = sizeof(platforms1)/sizeof(platforms1[0]);
	for (int i = 5; i < numberOfPlatforms; i++) {
		platforms1[i].startX = platformSetup[i-5].startX;
		platforms1[i].prevStartX = platformSetup[i-5].startX;
		platforms1[i].startY = platformSetup[i-5].startY;
		platforms1[i].prevStartY = platformSetup[i-5].startY;
		platforms1[i].colour = platformSetup[i-5].colour;
		platforms1[i].height = platformSetup[i-5].height;
		platforms1[i].prevHeight = platformSetup[i-5].height;
		platforms1[i].width = platformSetup[i-5].width;
		platforms1[i].prevWidth = platformSetup[i-5].width;
		platformLocations1[i].startX = platformSetup[i-5].startX;
		platformLocations1[i].prevStartX = platformSetup[i-5].startX;
		platformLocations1[i].startY = platformSetup[i-5].startY;
		platformLocations1[i].prevStartY = platformSetup[i-5].startY;
		platformLocations1[i].colour = platformSetup[i-5].colour;
		platformLocations1[i].height = platformSetup[i-5].height;
		platformLocations1[i].prevHeight = platformSetup[i-5].height;
		platformLocations1[i].width = platformSetup[i-5].width;
		platformLocations1[i].prevWidth = platformSetup[i-5].width;
		platforms2[i].startX = platformSetup[i-5].startX;
		platforms2[i].prevStartX = platformSetup[i-5].startX;
		platforms2[i].startY = platformSetup[i-5].startY-topBottomYDifference;
		platforms2[i].prevStartY = platformSetup[i-5].startY-topBottomYDifference;
		platforms2[i].colour = platformSetup[i-5].colour;
		platforms2[i].height = platformSetup[i-5].height;
		platforms2[i].prevHeight = platformSetup[i-5].height;
		platforms2[i].width = platformSetup[i-5].width;
		platforms2[i].prevWidth = platformSetup[i-5].width;
		platformLocations2[i].startX = platformSetup[i-5].startX;
		platformLocations2[i].prevStartX = platformSetup[i-5].startX;
		platformLocations2[i].startY = platformSetup[i-5].startY-topBottomYDifference;
		platformLocations2[i].prevStartY = platformSetup[i-5].startY-topBottomYDifference;
		platformLocations2[i].colour = platformSetup[i-5].colour;
		platformLocations2[i].height = platformSetup[i-5].height;
		platformLocations2[i].prevHeight = platformSetup[i-5].height;
		platformLocations2[i].width = platformSetup[i-5].width;
		platformLocations2[i].prevWidth = platformSetup[i-5].width;
	}	
	
	// Main loop
	while (1) {
		// Get key presses
		updateKeys();
		
		// Erase previous players		
		drawRectangle(player1.pastX, player1.pastY, playerSize, playerSize, BLACK);
		drawRectangle(player2.pastX, player2.pastY, playerSize, playerSize, BLACK);
		erasePlatforms(platforms1, numberOfPlatforms);
		erasePlatforms(platforms2, numberOfPlatforms);
		
		drawScore(borderEndY + 5, player1.prevScore, BLACK, digits);
		drawScore(yMin + 5 + platformSize, player2.prevScore, BLACK, digits);
		
		// Update past variables
		player1.pastX = player1.x;
		player1.pastY = player1.y;
		
		player2.pastX = player2.x;
		player2.pastY = player2.y;
		
		player1.prevScore = player1.score;
		player2.prevScore = player2.score;

        for (int i = 3; i < numberOfPlatforms; i++) {
            platforms1[i].prevStartX = platforms1[i].startX;
            platforms1[i].prevStartY = platforms1[i].startY;
            platforms1[i].prevWidth = platforms1[i].width;
            platforms1[i].prevHeight = platforms1[i].height;
			platforms2[i].prevStartX = platforms2[i].startX;
            platforms2[i].prevStartY = platforms2[i].startY;
            platforms2[i].prevWidth = platforms2[i].width;
            platforms2[i].prevHeight = platforms2[i].height;
        }
		
		// Move players
		movePlayer(&player1, gravity, platforms1, numberOfPlatforms, platformLocations1);
		movePlayer(&player2, gravity, platforms2, numberOfPlatforms, platformLocations2);
		
		// Draw stuff
		drawPlatforms(platforms1, numberOfPlatforms);
		drawPlatforms(platforms2, numberOfPlatforms);
		drawRectangle(player1.x, player1.y, playerSize, playerSize, player1.colour);
		drawRectangle(player2.x, player2.y, playerSize, playerSize, player2.colour);
		
		drawScore(borderEndY + 5, player1.score, WHITE, digits);
		drawScore(yMin + 5 + platformSize, player2.score, WHITE, digits);
		
		// Switch buffers
		wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	}
}

void plot_pixel(int x, int y, short int line_color) {
	volatile short int *one_pixel_address;
	one_pixel_address = (short int*) (pixel_buffer_start + (y << 10) + (x << 1));
	*one_pixel_address = line_color;
}

void wait_for_vsync() {
	volatile int* pixel_ctrl_ptr = (int*) 0xFF203020;
	(*pixel_ctrl_ptr) = 1;
	int statusRegister;
	int statusBit;
	while (1) {
		statusRegister = *(pixel_ctrl_ptr + 3);
		statusBit = (statusRegister & 1);	
		if (statusBit == 0) {
			return;	
		}
	}
}

void swap(int* num1, int* num2) {
	int temp = (*num1);
	(*num1) = (*num2);
	(*num2) = temp;
}

void checkKey(bool* keyBool, unsigned char code, bool extendedChar) {
	if (key1 == code && key2 != 0xF0) {
		(*keyBool) = true;	
	}
	if (key1 == code && key2 == 0xF0) {
		(*keyBool) = false;	
	}
	if (extendedChar) {
		if (key1 == 0x72 && key2 == 0xE0 && key3 != 0xF0) {
			(*keyBool) = true;	
		}
		if (key1 == 0x72 && key2 == 0xE0 && key3 != 0xF0) {
			(*keyBool) = true;	
		}
	}
}

void updateKeys() {
	volatile int * PS2_ptr = (int *) 0xFF200100;
	int data;
	data = *PS2_ptr;
	if (data & 0x8000) {
		key3 = key2;
		key2 = key1;
		key1 = data & 0xFF;	
	}
	checkKey(&wPressed, 0x1D, false);
	checkKey(&aPressed, 0x1C, false);
	checkKey(&sPressed, 0x1B, false);
	checkKey(&dPressed, 0x23, false);
	checkKey(&upPressed, 0x75, true);
	checkKey(&downPressed, 0x72, true);
	checkKey(&leftPressed, 0x6B, true);
	checkKey(&rightPressed, 0x74, true);
}

void setSolidScreen(short int colour) {
	for (int x = xMin; x < xSize; x++) {
		for (int y = 0; y < ySize; y++) {
			plot_pixel(x, y, colour);	
		}
	}
}

void drawRectangle(int xStart, int yStart, int width, int height, short int colour) {
	for (int x = xStart; x < width+xStart; x++) {
		for (int y = yStart; y < height+yStart; y++) {
			plot_pixel(x, y, colour);	
		}
	}
}

void drawPlatforms(struct platform platforms[], int numberOfPlatforms) {
	for (int i = 0; i < numberOfPlatforms; i++) {
		if (platforms[i].startX >= xMin && platforms[i].startX + platforms[i].width <= xSize) {
			drawRectangle(platforms[i].startX, platforms[i].startY, platforms[i].width, platforms[i].height, platforms[i].colour);	
		}
	}			
}

void erasePlatforms(struct platform platforms[], int numberOfPlatforms) {
	for (int i = 3; i < numberOfPlatforms; i++) {
		drawRectangle(platforms[i].prevStartX, platforms[i].prevStartY, platforms[i].prevWidth, platforms[i].prevHeight, BLACK);	
	}
}

void movePlayer(struct Player* player, int gravity, struct platform platforms[], int numberOfPlatforms, struct platform platformLocations[]) {
	int topBoundAbove, bottomBoundAbove, topBoundBelow, bottomBoundBelow;
	int xBoundLeft, xBoundRight;
	if (*(player->upControl)) {
        for (int i = 0; i < numberOfPlatforms; i++) {
            if (player->y == (platforms[i].startY-playerSize)) {
                if (player->x+playerSize > platforms[i].startX && player->x < (platforms[i].startX + platforms[i].width)) {
                    player->yVelocity = player->jumpSpeed;
                }
            }
        }
	}

	player->yVelocity += gravity;
	
    for (int i = 0; i < numberOfPlatforms; i++) {
		topBoundAbove = platforms[i].startY-playerSize-abs(player->yVelocity);
		bottomBoundAbove = platforms[i].startY+platforms[i].height;
		topBoundBelow = platforms[i].startY+platforms[i].height;
		bottomBoundBelow = platforms[i].startY+platforms[i].height+abs(player->yVelocity);
		xBoundLeft = platforms[i].startX;
		xBoundRight = platforms[i].startX + platforms[i].width;
		
		if (player->x+playerSize > xBoundLeft && player->x < xBoundRight) {
			if (player->y >= topBoundAbove && player->y <= bottomBoundAbove && player->yVelocity > 0) {
				player->y = platforms[i].startY-playerSize;
				player->yVelocity = 0;
			} else if (player->y >= topBoundBelow && player->y <= bottomBoundBelow && player->yVelocity < 0) {
				player->y = platforms[i].startY+platforms[i].height+1;
				player->yVelocity = 0;
			}
		}
    }
	player->y += player->yVelocity;
	int difference;
	bool adjustment = false;
	int extraDistance;
	if ((*(player->leftControl))) {
		bool hitLeftWall = false;
		for (int i = 3; i < numberOfPlatforms; i++) {
			if (player->x >= platforms[i].startX+platforms[i].width && player->x - player->speed <= platforms[i].startX+platforms[i].width) {
				if (player->y+playerSize > platforms[i].startY && player->y < platforms[i].startY + platforms[i].height) {
					hitLeftWall = true;
					extraDistance = player->x - (platforms[i].startX + platforms[i].width);
					if (extraDistance > 0) {
						hitLeftWall = false;
						adjustment = true;
					}
					break;
				}
			}
		}
		if (hitLeftWall == false) {
			for (int i = 3; i < numberOfPlatforms; i++) {
				if (adjustment) {
					platformLocations[i].startX += extraDistance;	
				} else {
					platformLocations[i].startX += player->speed;
				}
				if (platformLocations[i].startX > xSize) {
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = 0;
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = xSize;
				} else if (platformLocations[i].startX + platformLocations[i].width > xSize) {
					difference = (platformLocations[i].startX + platformLocations[i].width) - xSize;
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = platformLocations[i].width - difference;
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = platformLocations[i].startX;
				} else if (platformLocations[i].startX <= xMin && platformLocations[i].startX + platformLocations[i].width >= xMin) {
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = xMin;
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = platformLocations[i].startX + platformLocations[i].width;
				} else {
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = platformLocations[i].width;
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = platformLocations[i].startX;
				}
			}
		}
	}
	if ((*(player->rightControl))) {
		bool hitRightWall = false;
		bool adjustment = false;
		int playerRight = player->x + playerSize;
		int platformLeft;
		int extraDistance;
		int difference;
		for (int i = 3; i < numberOfPlatforms; i++) {
			platformLeft = platforms[i].startX;
			if (playerRight <= platformLeft && playerRight + player->speed >= platformLeft) {
				if (player->y+playerSize > platforms[i].startY && player->y < platforms[i].startY + platforms[i].height) {
					extraDistance = platforms[i].startX - (player->x+playerSize);
					hitRightWall = true;
					if (i == 4) {
						player->score++;	
					}
					if (extraDistance > 0) {
						hitRightWall = false;
						adjustment = true;
					}
					break;
				}
			}
		}
		if (hitRightWall == false) {
			for (int i = 3; i < numberOfPlatforms; i++) {
				if (adjustment) {
					platformLocations[i].startX -= extraDistance;
				} else {
					platformLocations[i].startX -= player->speed;
				}
				if (platformLocations[i].startX < xMin) {
					if (platformLocations[i].startX + platformLocations[i].width < xMin) {
						// platforms[i].prevWidth = platforms[i].width;
						platforms[i].width = 0;
					} else {
						// platforms[i].prevWidth = platforms[i].width;
						platforms[i].width = platformLocations[i].startX + platformLocations[i].width;
					}
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = xMin;
				} else if (platformLocations[i].startX >= xMin && platformLocations[i].startX + platformLocations[i].width <= xSize) {
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = platformLocations[i].width;
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = platformLocations[i].startX;
				} else {
					difference = xSize - platformLocations[i].startX;
					// platforms[i].prevWidth = platforms[i].width;
					platforms[i].width = difference;
					// platforms[i].prevStartX = platforms[i].startX;
					platforms[i].startX = platformLocations[i].startX;
				}
			}
		}
	}
}

void drawDigit(int number[], int startX, int startY, short int colour) {
	int index = 0;
	for (int y = startY; y < startY+5; y++) {
		for (int x = startX; x < startX+3; x++) {
			if (number[index]) {
				plot_pixel(x, y, colour);
			}
			index++;
		}
	}
}

void drawScore(int y, int score, short int colour, int digits[10][15]) {
	int x = xSize - 4;
	int digit;
	if (score == 0) {
		drawDigit(zero, x, y, colour);
		return;
	}
	while (score) {
		digit = score % 10;
		drawDigit(digits[digit], x, y, colour);
		score /= 10;
		x -= 4;
	}
}










