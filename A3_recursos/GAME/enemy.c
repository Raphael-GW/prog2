#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"

// cria o inimigo
Enemy* createEnemy(int side_x, int side_y, int x, int y, int X_SCREEN, int Y_SCREEN){
    Enemy *e = (Enemy*)malloc(sizeof(Enemy));
    if (!e) return NULL;
    /* inicializa campos usados por main.c */
    e->side_x = side_x;
    e->side_y = side_y;
    e->x = x;
    e->y = y;
    e->max_x = X_SCREEN;
    e->max_y = Y_SCREEN;

    return e;
}

void deleteEnemy(Enemy *e){
    if (!e) return ;

    free (e);
}