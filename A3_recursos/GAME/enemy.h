#ifndef __ENEMY__
#define __ENEMY__

#define ENEMY_STEP 10
#define ALTURA_ENEMY 40
#define LARGURA_ENEMY 20
#define ENEMY_SPEED 3
#define GRAVITY 0.8f
#define JUMP_VELOCITY_ENM -12.0f

typedef struct {
    unsigned char side_x;					//Tamanho da lateral de um quadrado
	unsigned char side_y;                   //Tamanho vertical de um quadradro
    unsigned short x;								//Posição X do centro do quadrado
	unsigned short y;									//Posição Y do centro do quadrado
	unsigned short max_x;
    unsigned short max_y;
    
} Enemy;

typedef struct {
    Enemy *e;
    float vy;
    int ground_y;
    bool in_air;
} PhysicsEnemy;

Enemy* createEnemy(int side_x, int side_y, int x, int y, int X_SCREEN, int Y_SCREEN);
void deleteEnemy(Enemy *e);
void updateEnemyPhysics(Enemy *e);
PhysicsEnemy* find_entry(Enemy *e);
PhysicsEnemy* create_entry(Enemy *e, int ground_y);

#endif