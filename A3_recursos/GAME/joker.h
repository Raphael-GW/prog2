#ifndef __JOKER__
#define __JOKER__

#include "joystick.h"

#define VIDA_JOKER 3
#define LARGURA_JOKER 28
#define ALTURA_JOKER 53


#define GRAVITY 0.8f
#define JUMP_VELOCITY -12.0f
#define HORIZONTAL_SPEED 5


typedef struct {
    unsigned char side_x;					//Tamanho da lateral de um quadrado
	unsigned char side_y;                   //Tamanho vertical de um quadradro
    unsigned short x;								//Posição X do centro do quadrado
	unsigned short y;									//Posição Y do centro do quadrado
	unsigned short max_x;
    unsigned short max_y;
    int vida;
    bool flip;
    bool is_crouching;                     //Flag para indicar se está agachando
    
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