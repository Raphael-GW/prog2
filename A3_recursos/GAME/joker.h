#ifndef __JOKER__
#define __JOKER__

#include "joystick.h"

#define JOKER_STEP 10
#define V_JOKER_JUMP 0.5
#define VEL_Y 0
#define ALTURA_JOKER 20
#define CHAO_Y 220


#define GRAVITY 0.8f
#define JUMP_VELOCITY -12.0f
#define HORIZONTAL_SPEED 5


typedef struct {
    unsigned char side_x;					//Tamanho da lateral de um quadrado
	unsigned char side_y;
    unsigned short x;								//Posição X do centro do quadrado
	unsigned short y;									//Posição Y do centro do quadrado
	unsigned short max_x;
    unsigned short max_y;
    
    joystick *control;
} Joker;

typedef struct {
    Joker *j;
    float vy;
    int ground_y;
    bool in_air;
} PhysicsEntry;

Joker* createJoker(int side_x, int side_y, int x, int y, int X_SCREEN, int Y_SCREEN);
void deleteJoker(Joker *j);
void moveJoker(Joker *j, int amount, int dir, int X_SCREEN, int Y_SCREEN);
void updateJokerPhysics(Joker *j);


#endif