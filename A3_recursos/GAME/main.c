//Compilação: gcc AggressiveJokers.c joker.c joystick.c -o AS $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 --libs --cflags) (!)

#include <allegro5/allegro5.h>																																											//Biblioteca base do Allegro
#include <allegro5/allegro_font.h>																																										//Biblioteca de fontes do Allegro
#include <allegro5/allegro_primitives.h>																																								//Biblioteca de figuras básicas
#include <allegro5/allegro_ttf.h>

#include "joker.h"
#include "enemy.h"

#include <stdio.h>																																														//Inclusão da biblioteca de quadrados

#define X_SCREEN 640																																													//Definição do tamanho da tela em pixels no eixo x
#define Y_SCREEN 320																																													//Definição do tamanho da tela em pixels no eixo y


unsigned char collision_2D(Joker *element_first, Enemy *element_second){

    if ((((element_first->x+element_first->side_x/2 >= element_second->x-element_second->side_x/2) && (element_second->x-element_second->side_x/2 >= element_first->x-element_first->side_x/2)) || 	//			//Verifica se o primeiro elemento colidiu com o segundo no eixo X			
        ((element_second->x+element_second->side_x/2 >= element_first->x-element_first->side_x/2) && (element_first->x-element_first->side_x/2 >= element_second->x-element_second->side_x/2))) && 	//			//Verifica se o segundo elemento colidiu com o primeiro no eixo X
        (((element_first->y+element_first->side_y/2 >= element_second->y-element_second->side_y/2) && (element_second->y-element_second->side_y/2 >= element_first->y-element_first->side_y/2)) || 	//			//Verifica se o primeiro elemento colidiu com o segundo no eixo Y
        ((element_second->y+element_second->side_y/2 >= element_first->y-element_first->side_y/2) && (element_first->y-element_first->side_y/2 >= element_second->y-element_second->side_y/2)))) return 1;		//Verifica se o segundo elemento colidiu com o primeiro no eixo Y
    else return 0;
}

void update_position(Joker *player_1){																																				//Função de atualização das posições dos quadrados conforme os comandos do controle (!)
    if (player_1->control->left){																																										//Se o botão de movimentação para esquerda do controle do primeiro jogador está ativado... (!)
        moveJoker(player_1, 1, 0, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a esquerda (!)
    }
    if (player_1->control->right){																																										//Se o botão de movimentação para direita do controle do primeir ojogador está ativado... (!)
        moveJoker(player_1, 1, 1, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a direta (!)
    }
    if (player_1->control->jump) {																																										//Se o botão de movimentação para cima do controle do primeiro jogador está ativado... (!)
        moveJoker(player_1, 1, 2, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para cima (!)
    }
    if (player_1->control->down){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
        moveJoker(player_1, 1, 3, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
    }
}

int main(){
    if (!al_init()) {
        fprintf(stderr, "al_init failed\n");
        return 1;
    }

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    if (!al_install_keyboard()) {
        fprintf(stderr, "al_install_keyboard failed\n");
        return 1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    if (!timer) { fprintf(stderr, "failed to create timer\n"); return 1; }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if (!queue) { fprintf(stderr, "failed to create event queue\n"); al_destroy_timer(timer); return 1; }

    ALLEGRO_DISPLAY* disp = al_create_display(X_SCREEN, Y_SCREEN);
    if (!disp) { fprintf(stderr, "failed to create display\n"); al_destroy_event_queue(queue); al_destroy_timer(timer); return 1; }

    ALLEGRO_FONT* font = al_load_ttf_font("P5font.TTF", 24, 0);
    ALLEGRO_FONT* bigfont = al_load_ttf_font("P5font.TTF", 48, 0);
    if (!font || !bigfont) {
        fprintf(stderr, "failed to load font(s). Verifique se BRADHITC.TTF está no diretório do executável\n");
        /* continua, mas sem fontes o desenho de texto irá falhar; prefira encerrar */
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());																																	//Indica que eventos de teclado serão inseridos na nossa fila de eventos
    al_register_event_source(queue, al_get_display_event_source(disp));																																	//Indica que eventos de tela serão inseridos na nossa fila de eventos
    al_register_event_source(queue, al_get_timer_event_source(timer));																																	//Indica que eventos de relógio serão inseridos na nossa fila de eventos

    Joker* player_1 = createJoker(20, ALTURA_JOKER, 10, CHAO_Y, X_SCREEN, Y_SCREEN);																															//Cria o quadrado do primeiro jogador
    if (!player_1) return 1;																																											//Verificação de erro na criação do quadrado do primeiro jogador
    Enemy* enemy = createEnemy(20, ALTURA_ENEMY, X_SCREEN-10, CHAO_Y, X_SCREEN, Y_SCREEN);																													//Cria o quadrado do segundo jogador
    if (!enemy) return 2;																																											//Verificação de erro na criação do quadrado do segundo jogador

    ALLEGRO_EVENT event;																																												//Variável que guarda um evento capturado, sua estrutura é definida em: https:		//www.allegro.cc/manual/5/ALLEGRO_EVENT
    al_start_timer(timer);																																												//Função que inicializa o relógio do programa

    int collision_cooldown = 0; /* frames de invulnerabilidade após levar dano (ex: 30 = 1s) */
    int game_over = 0;

    while(1){																																															//Laço principal do programa
        al_wait_for_event(queue, &event);																																								//Função que captura eventos da fila, inserindo os mesmos na variável de eventos

        if (event.type == 30){
            /* Atualizações ocorram apenas se não for game over */
            if (!game_over) {
                /* atualiza inputs (movimentação) */
                update_position(player_1);

                /* decrementa cooldown se existir */
                if (collision_cooldown > 0) collision_cooldown--;

                // verifica colisão e aplica dano + recuo 3 passos (com cooldown)
                if (collision_2D(player_1, enemy) && collision_cooldown == 0) {
                    player_1->vida--;
                    collision_cooldown = 30; /* 1 segundo de invulnerabilidade */

                    /* recuo de 3 passos para a esquerda (chama moveJoker 3x com passo simples) */
                    for (int i = 0; i < 3; ++i) {
                        moveJoker(player_1, 1, 0, X_SCREEN, Y_SCREEN);
                    }

                    if (player_1->vida <= 0) {
                        player_1->vida = 0;
                        game_over = 1;
                    }
                }

                /* atualiza física do jogador */
                updateJokerPhysics(player_1);
            }

            /* desenha cena */
            if (game_over) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(bigfont, al_map_rgb(255, 0, 0), X_SCREEN / 2, 90, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
                al_draw_textf(font, al_map_rgb(255,255,255), X_SCREEN / 2, 150, ALLEGRO_ALIGN_CENTRE, "Vida: %d", player_1->vida);
                al_draw_text(font, al_map_rgb(255,255,255), X_SCREEN / 2, 200, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
            } else {
                al_clear_to_color(al_map_rgb(255, 0, 0));																																						//Substitui tudo que estava desenhado na tela por um fundo
                al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "Vida: %hu", player_1->vida);
                al_draw_filled_rectangle(player_1->x-player_1->side_x/2, player_1->y-player_1->side_y/2, player_1->x+player_1->side_x/2, player_1->y+player_1->side_y/2, al_map_rgb(0, 255, 0));					//Insere o quadrado do primeiro jogador na tela
                al_draw_filled_rectangle(enemy->x-enemy->side_x/2, enemy->y-enemy->side_y/2, enemy->x+enemy->side_x/2, enemy->y+enemy->side_y/2, al_map_rgb(0, 0, 255));					//Insere o quadrado do segundo jogador na tela
            }

            al_flip_display();																																											//Insere as modificações realizadas nos buffers de tela
        }
        else if ((event.type == 10) || (event.type == 12)){																																				//Verifica se o evento é de botão do teclado abaixado ou levantado (!)
            int pressed = (event.type == 10);

            if (event.keyboard.keycode == 1) player_1->control->left  = pressed;
            else if (event.keyboard.keycode == 4) player_1->control->right = pressed;
            else if (event.keyboard.keycode == 23) player_1->control->jump  = pressed;
            /* PLAYER 1 - agachar enquanto tecla está pressionada */
            else if (event.keyboard.keycode == 19) {
                if (pressed) {                 /* key down */
                    player_1->control->down = 1;
                    if(player_1->side_y == ALTURA_JOKER)
                        player_1->side_y = ALTURA_JOKER / 2; /* reduz a altura enquanto agacha */
                    player_1->side_y = player_1->side_y;
                } else {                       /* key up */
                    player_1->control->down = 0;
                    player_1->side_y = ALTURA_JOKER;   /* restaura altura original */
                }
            }

            /* permite sair no game over apertando ESC */
            if (game_over && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && !pressed) { /* ESC key up */
                break;
            }

        }
        else if (event.type == 42) break;																																								//Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
    }

    deleteEnemy (enemy);
    deleteJoker (player_1);
    al_destroy_font(font);																																												//Destrutor da fonte padrão
    al_destroy_font(bigfont);
    al_destroy_display(disp);																																											//Destrutor da tela
    al_destroy_timer(timer);																																											//Destrutor do relógio
    al_destroy_event_queue(queue);																																										//Destrutor da fila

    return 0;
}