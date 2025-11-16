#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "joker.h"



#define MAX_JOKERS 16
static PhysicsEntry physics_table[MAX_JOKERS];

static PhysicsEntry* find_entry(Joker *j){
    for(int i=0;i<MAX_JOKERS;i++){
        if (physics_table[i].j == j) return &physics_table[i];
    }
    return NULL;
}

static PhysicsEntry* create_entry(Joker *j, int ground_y){
    for(int i=0;i<MAX_JOKERS;i++){
        if (physics_table[i].j == NULL){
            physics_table[i].j = j;
            physics_table[i].vy = 0.0f;
            physics_table[i].ground_y = ground_y;
            physics_table[i].in_air = false;
            return &physics_table[i];
        }
    }
    return NULL;
}

/* Cria um Joker (assinatura usada em main.c) */
Joker* createJoker(int side_x, int side_y, int x, int y, int X_SCREEN, int Y_SCREEN){
    Joker *j = (Joker*)malloc(sizeof(Joker));
    if (!j) return NULL;
    /* inicializa campos usados por main.c */
    j->side_x = side_x;
    j->side_y = side_y;
    j->x = x;
    j->y = y;
    j->vida = VIDA_JOKER;
    j->is_crouching = false;  /* começa sem agachar */
    j->control = joystick_create ();
    if (!j->control){

    }
    j->max_x = X_SCREEN;
    j->max_y = Y_SCREEN;
    /* registra entrada de física */
    if (!create_entry(j, y)){
        /* limite excedido, mas devolve o joker mesmo assim */
    }
    return j;
}

/* Move o Joker: dirs: 0=left,1=right,2=jump,3=down
   amount >0 = aplicar movimento, amount<0 = desfazer (undo)
*/
void moveJoker(Joker *j, int amount, int dir, int X_SCREEN, int Y_SCREEN){
    if (!j) return;
    PhysicsEntry *pe = find_entry(j);
    if (!pe) {
        /* se não existir entrada, cria com ground igual à posição atual */
        pe = create_entry(j, j->y);
    }

    if (dir == 0){ /* left */
        if (j->x - j->side_x/2 <= 0) j->x = j->side_x/2;
        else j->x -= HORIZONTAL_SPEED * (amount>0?1:-1);
    } else if (dir == 1){ /* right */
        if (j->x + j->side_x/2 >= X_SCREEN) j->x = X_SCREEN - j->side_x/2;
        else j->x += HORIZONTAL_SPEED * (amount>0?1:-1);
    } else if (dir == 2){ /* jump -> apenas inicia velocidade vertical se estiver no chão */
        if (amount > 0){
            if (!pe->in_air){
                pe->vy = JUMP_VELOCITY;
                pe->in_air = true;
            }
        } else {
            /* undo jump: não precisa desfazer nada no caso de física */
        }
    } else if (dir == 3){
        /* agachar: amount > 0 = agachar, amount < 0 = levantar */
        if (amount > 0){
            /* inicia agachamento */
            if (!j->is_crouching){
                j->is_crouching = true;
                j->side_y = (unsigned char)(ALTURA_JOKER / 2);  /* reduz altura */
                j->y += ALTURA_JOKER / 4;      /* desce ligeiramente para manter pés no chão */
            }
        } else {
            /* encerra agachamento */
            if (j->is_crouching){
                j->is_crouching = false;
                j->y -= ALTURA_JOKER / 4;      /* sobe de volta */
                j->side_y = (unsigned char)ALTURA_JOKER;      /* restaura altura original */
            }
        }
    }
}

/* Chamado todo tick para aplicar gravidade e atualizar y */
void updateJokerPhysics(Joker *j){
    if (!j) return;
    PhysicsEntry *pe = find_entry(j);
    if (!pe) {
        pe = create_entry(j, j->y);
        if (!pe) return;
    }

    if (pe->in_air){
        pe->vy += GRAVITY;
        /* atualiza posição vertical (j->y pode ser int) */
        j->y += (int)(pe->vy);
        /* se passou do chão, corrige e para o salto */
        if (j->y >= pe->ground_y){
            j->y = pe->ground_y;
            pe->vy = 0.0f;
            pe->in_air = false;
        }
    } else {
        /* garante que esteja no chão */
        if (j->y < pe->ground_y){
            /* caiu de alguma forma: ativa queda */
            pe->in_air = true;
        }
    }
}

/* Opcional: função para liberar recursos (não usada no main atual) */
void deleteJoker(Joker *j){
    if (!j) return;
    /* remove da tabela física */
    for(int i=0;i<MAX_JOKERS;i++){
        if (physics_table[i].j == j){
            physics_table[i].j = NULL;
            physics_table[i].vy = 0.0f;
            physics_table[i].in_air = false;
            physics_table[i].ground_y = 0;
        }
    }
    if (j->control) free(j->control);
    free(j);
}