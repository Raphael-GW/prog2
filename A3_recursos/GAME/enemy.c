#include <stdlib.h>
#include <stdbool.h>
#include "enemy.h"


#define MAX_ENEMY 16
PhysicsEnemy physics_table[MAX_ENEMY];

PhysicsEnemy* find_entry(Enemy *e){
    for(int i=0;i<MAX_ENEMY;i++){
        if (physics_table[i].e == e) return &physics_table[i];
    }
    return NULL;
}

PhysicsEnemy* create_entry(Enemy *e, int ground_y){
    for(int i=0;i<MAX_ENEMY;i++){
        if (physics_table[i].e == NULL){
            physics_table[i].e = e;
            physics_table[i].vy = 0.0f;
            physics_table[i].ground_y = ground_y;
            physics_table[i].in_air = false;
            return &physics_table[i];
        }
    }
    return NULL;
}

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
    create_entry(e, y);

    return e;
}


/* Chamado todo tick para aplicar gravidade e atualizar y */
void updateEnemyPhysics(Enemy *e){
    if (!e) return;
    PhysicsEnemy *pe = find_entry(e);
    if (!pe) {
        pe = create_entry(e, e->y);
        if (!pe) return;
    }

    if (pe->in_air){
        pe->vy += GRAVITY;
        /* atualiza posição vertical (j->y pode ser int) */
        e->y += (int)(pe->vy);
        /* se passou do chão, corrige e para o salto */
        if (e->y >= pe->ground_y){
            e->y = pe->ground_y;
            pe->vy = 0.0f;
            pe->in_air = false;
        }
    } else {
        /* garante que esteja no chão */
        if (e->y < pe->ground_y){
            /* caiu de alguma forma: ativa queda */
            pe->in_air = true;
        }
    }
}

void deleteEnemy(Enemy *e){
    if (!e) return ;

    free (e);
}