#include <stdlib.h>
#include "joker.h"

Joker* createJoker(unsigned char side_x, unsigned char side_y, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y){
    Joker *j = (Joker*) malloc (sizeof(Joker));										//Aloca memória na heap para um novo Joker
    if (!j) return NULL;															//Se a alocação não deu certo, retorna erro
    j->side_x = side_x;															    //Define o tamanho da lateral do quadrado
    j->side_y = side_y;                                                             //Define o tamanho da vertical do quadrado
    j->x = x;																		//Define a posição X do centro do quadrado
    j->y = y;																		//Define a posição Y do centro do quadrado
    j->max_x = max_x;																//Define a posição máxima no eixo X
    j->max_y = max_y;																//Define a posição máxima no eixo Y
    j->control = joystick_create();
    if (!j->control) { /* failed to allocate control: clean up and return error */
        free(j);
        return NULL;
    }

    return j;																		//Retorna o novo Joker
}

void deleteJoker(Joker *j){
    if (!j) return;
    if (j->control) joystick_destroy(j->control);									//Destrói o controle associado ao Joker (só se existir)
    free(j);																		//Libera a memória do Joker na heap
}

void moveJoker(Joker *j, char steps, unsigned char direction, unsigned short maxX, unsigned short maxY){
    switch(direction){
        case 1: // Move para a direita
            if (j->x + steps*JOKER_STEP + j->side_x / 2 <= maxX) {
                j->x += steps*JOKER_STEP;
            }
            break;
        case 0: // Move para a esquerda
            if (j->x - steps*JOKER_STEP - j->side_x / 2 >= 0) {
                j->x -= steps*JOKER_STEP;
            }
            break;
        case 2: // Move para cima
            if (j->y - steps*JOKER_STEP - j->side_y / 2 >= 0) {
                j->y -= steps*JOKER_STEP;
            }
            break;
        default:
            break;
    }
}