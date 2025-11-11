#ifndef __ENEMY__
#define __ENEMY__

#define ENEMY_STEP 10
#define ALTURA_ENEMY 30
#define LARGURA_ENEMY 20


typedef struct {
    unsigned char side_x;					//Tamanho da lateral de um quadrado
	unsigned char side_y;                   //Tamanho vertical de um quadradro
    unsigned short x;								//Posição X do centro do quadrado
	unsigned short y;									//Posição Y do centro do quadrado
	unsigned short max_x;
    unsigned short max_y;
    
} Enemy;

Enemy* createEnemy(int side_x, int side_y, int x, int y, int X_SCREEN, int Y_SCREEN);
void deleteEnemy(Enemy *e);

#endif