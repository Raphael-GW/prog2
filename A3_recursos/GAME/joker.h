#ifndef __JOKER__
#define __JOKER__

#include "joystick.h"

#define JOKER_STEP 10

typedef struct {
    unsigned char side_x;					//Tamanho da lateral de um quadrado
	unsigned char side_y;
    unsigned short x;								//Posição X do centro do quadrado
	unsigned short y;									//Posição Y do centro do quadrado
	unsigned short max_x;
    unsigned short max_y;
    joystick *control;
} Joker;

Joker* createJoker(unsigned char side_x, unsigned char side_y, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y);
void deleteJoker(Joker *j);
void moveJoker(Joker *j, char steps, unsigned char direction, unsigned short maxX, unsigned short maxY);

#endif