#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
	
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

volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

int xSize = 320;
int ySize = 240;

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
};

void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void swap(int* num1, int* num2);
void checkKey(bool* keyBool, unsigned char code, bool extendedChar);
void updateKeys();
void setSolidScreen(short int colour);
void drawRectangle(int xStart, int yStart, int width, int height, short int colour);
void drawPlatforms(struct platform platforms[], int numberOfPlatforms);
void movePlayer(struct Player* player, int gravity, struct platform platforms[], int numberOfPlatforms);

int main(void) {
	// Setup
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
    wait_for_vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
    setSolidScreen(BLACK);
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	setSolidScreen(BLACK);
	(*pixel_ctrl_ptr) = 1;
	
	// Player setup
    struct Player player1 = {6, ySize/2+15, 6, ySize/2+15, 3, -10, 0, &wPressed, &sPressed, &aPressed, &dPressed, RED};
    struct Player player2 = {6, 6, 6, 6, 3, -10, 0, &upPressed, &downPressed, &leftPressed, &rightPressed, WHITE};
	
	// Screen Border and divider
	struct platform platform1 = {0, 0, xSize, platformSize, BLUE};
	struct platform platform2 = {0, (ySize/2-platformSize), xSize, platformSize*2, BLUE};
	struct platform platform3 = {0, ySize-platformSize, xSize, platformSize, BLUE};
	struct platform platform4 = {0, 0, platformSize, ySize, BLUE};
	struct platform platform5 = {xSize-platformSize, 0, platformSize, ySize, BLUE};
	
	// Jump platforms
	struct platform platform6 = {100, 200, 50, platformSize, GREEN};
	struct platform platform7 = {175, 175, 75, platformSize, GREEN};
	
	struct platform platform8 = {100, 200-ySize/2, 50, platformSize, GREEN};
	struct platform platform9 = {175, 175-ySize/2, 75, platformSize, GREEN};
	
	struct platform platforms[] = {platform1, platform2, platform3, platform4, platform5, platform6, platform7, platform8, platform9};

    int numberOfPlatforms = sizeof(platforms)/sizeof(platforms[0]);
	
	// Main loop
	int screenNumber = 0;
    setSolidScreen(BLACK);
	while (1) {
		// Get key presses
		updateKeys();
		
		// Erase previous players		
		drawRectangle(player1.pastX, player1.pastY, playerSize, playerSize, BLACK);
		drawRectangle(player2.pastX, player2.pastY, playerSize, playerSize, BLACK);
		
		// Update past variables
		player1.pastX = player1.x;
		player1.pastY = player1.y;
		
		player2.pastX = player2.x;
		player2.pastY = player2.y;
		
		// Move players
		movePlayer(&player1, gravity, platforms, numberOfPlatforms);
		movePlayer(&player2, gravity, platforms, numberOfPlatforms);
		
		// Draw stuff
		drawPlatforms(platforms, numberOfPlatforms);
		drawRectangle(player1.x, player1.y, playerSize, playerSize, player1.colour);
		drawRectangle(player2.x, player2.y, playerSize, playerSize, player2.colour);
		
		// Switch buffers
		wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	}
}

void plot_pixel(int x, int y, short int line_color) {
	volatile short int *one_pixel_address;
	one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);    
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
	for (int x = 0; x < xSize; x++) {
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
		drawRectangle(platforms[i].startX, platforms[i].startY, platforms[i].width, platforms[i].height, platforms[i].colour);
	}			
}

void movePlayer(struct Player* player, int gravity, struct platform platforms[], int numberOfPlatforms) {
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
	if (player->y < ySize-playerSize-platformSize) {
		player->yVelocity += gravity;
	}
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
	if ((*(player->leftControl)) && player->x > platformSize) {
		player->x -= player->speed;
		if (player->x < platformSize) {
			player->x = platformSize;
		}
	}
	if ((*(player->rightControl)) && player->x < xSize-platformSize-playerSize) {
		player->x += player->speed;
		if (player->x > xSize-platformSize-playerSize) {
			player->x = xSize-platformSize-playerSize;	
		}
	}
}