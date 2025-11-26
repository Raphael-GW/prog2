#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include "joker.h"
#include "enemy.h"

#include <stdio.h>

#define X_SCREEN 1025
#define Y_SCREEN 512
#define CHAO_Y (317 - ALTURA_JOKER/2)

/* Dimensões dos sprites na imagem */
#define SPRITE_WIDTH 28
#define SPRITE_HEIGHT 53

long aleat (long min, long max)
{
    return min + (rand () % ((max - min) + 1));
}

unsigned char collision_2D(Joker *element_first, Enemy *element_second){
    if ((((element_first->x+element_first->side_x/2 >= element_second->x-element_second->side_x/2) && (element_second->x-element_second->side_x/2 >= element_first->x-element_first->side_x/2)) || 	
        ((element_second->x+element_second->side_x/2 >= element_first->x-element_first->side_x/2) && (element_first->x-element_first->side_x/2 >= element_second->x-element_second->side_x/2))) && 	
        (((element_first->y+element_first->side_y/2 >= element_second->y-element_second->side_y/2) && (element_second->y-element_second->side_y/2 >= element_first->y-element_first->side_y/2)) || 	
        ((element_second->y+element_second->side_y/2 >= element_first->y-element_first->side_y/2) && (element_first->y-element_first->side_y/2 >= element_second->y-element_second->side_y/2)))) return 1;
    else return 0;
}

int background_x = 0;
void update_position(Joker *player_1){
    int half = player_1->side_x / 2;
    /* permite mover o jogador livremente dentro da tela (entre half e X_SCREEN-half) */
    if (player_1->control->left){
        if (player_1->x > half) {
            moveJoker(player_1, 1, 0, X_SCREEN, Y_SCREEN);
            player_1->flip = true;
        }
    }
    if (player_1->control->right){
        if (player_1->x < X_SCREEN - half) {
            moveJoker(player_1, 1, 1, X_SCREEN, Y_SCREEN);
            player_1->flip = false;
        }
    }
    if (player_1->control->jump) {
        moveJoker(player_1, 1, 2, X_SCREEN, Y_SCREEN);
    }
    if (player_1->control->down){
        moveJoker(player_1, 1, 3, X_SCREEN, Y_SCREEN);
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
    al_init_image_addon();

    if (!al_install_keyboard()) {
        fprintf(stderr, "al_install_keyboard failed\n");
        return 1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    if (!timer) {
        fprintf(stderr, "failed to create timer\n"); 
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "failed to create event queue\n"); al_destroy_timer(timer); 
        return 1;
    }

    ALLEGRO_DISPLAY* disp = al_create_display(X_SCREEN, Y_SCREEN);
    if (!disp) {
        fprintf(stderr, "failed to create display\n"); al_destroy_event_queue(queue); al_destroy_timer(timer); 
        return 1;
    }
    ALLEGRO_FONT* font = al_load_ttf_font("P5font.ttf", 24, 0);
    ALLEGRO_FONT* bigfont = al_load_ttf_font("P5font.ttf", 48, 0);
    if (!font || !bigfont) {
        fprintf(stderr, "failed to load font(s).\n");
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *fase = al_load_bitmap("stage.png");
    if (!fase) {
        fprintf(stderr, "failed to load bitmap.\n");
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }
    
    ALLEGRO_BITMAP *joker_sprite_sheet = al_load_bitmap("joker_sprite.png");
    if (!joker_sprite_sheet) {
        fprintf(stderr, "failed to load joker sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    /* Extrai sub-bitmaps de cada sprite (parado direita, parado esquerda, pulando, agachando) */
    ALLEGRO_BITMAP *sprite_idle_right = al_create_sub_bitmap(joker_sprite_sheet, 25, 190, SPRITE_WIDTH, SPRITE_HEIGHT);
    ALLEGRO_BITMAP *sprite_idle_left = al_create_sub_bitmap(joker_sprite_sheet, SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT);
    ALLEGRO_BITMAP *sprite_jump = al_create_sub_bitmap(joker_sprite_sheet, 16, 122, SPRITE_WIDTH, SPRITE_HEIGHT);
    ALLEGRO_BITMAP *sprite_crouch = al_create_sub_bitmap(joker_sprite_sheet, SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_WIDTH, SPRITE_HEIGHT);

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Joker* player_1 = createJoker(20, ALTURA_JOKER, 66, CHAO_Y, X_SCREEN, Y_SCREEN);
    if (!player_1) return 1;
    Enemy* enemy = createEnemy(20, ALTURA_ENEMY, X_SCREEN-10, CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy) return 2;
    Enemy* enemy_2 = createEnemy(10, ALTURA_ENEMY+10, aleat(X_SCREEN+30, 2*X_SCREEN), CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy_2) return 2;
    Enemy* enemy_3 = createEnemy(20, ALTURA_ENEMY+50, aleat(X_SCREEN+50, 2*X_SCREEN), CHAO_Y-62, X_SCREEN, Y_SCREEN);
    if (!enemy_3) return 2;
    Enemy* enemy_4 = createEnemy(100, 10, aleat(X_SCREEN+90, 2*X_SCREEN), CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2) + 15, X_SCREEN, Y_SCREEN);
    if (!enemy_4) return 2;
    Enemy* enemy_5 = createEnemy(20, ALTURA_ENEMY, X_SCREEN-10, CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy_5) return 2;
    Enemy* enemy_6 = createEnemy(20, ALTURA_ENEMY, X_SCREEN-10, CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy_6) return 2;

    ALLEGRO_EVENT event;
    al_start_timer(timer);

    int collision_cooldown = 0;
    int game_over = 0;
    int menu = 1;
    int bg_w = al_get_bitmap_width(fase);
    int min_background_x = X_SCREEN - bg_w;
    int win = 0;
    int up_down = 6;


    while(1){
        al_wait_for_event(queue, &event);
        while (menu) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(bigfont, al_map_rgb(255, 255, 255), X_SCREEN / 2, 150, ALLEGRO_ALIGN_CENTRE, "Prog2 - Game");
            al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, 250, ALLEGRO_ALIGN_CENTRE, "Pressione ENTER para iniciar");
            al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, 350, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
            al_flip_display();

            al_wait_for_event(queue, &event);
            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                menu = 0;
            }
            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                return 0;
            }
        }
        if (win){
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(bigfont, al_map_rgb(0, 255, 0), X_SCREEN / 2, 150, ALLEGRO_ALIGN_CENTRE, "VICTORY!!");
            al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, 350, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
            al_flip_display();

            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                return 0;
            }
        }
        if (event.type == 30 && !win){
            if (!game_over) {
                update_position(player_1);

                if (player_1->x >= X_SCREEN - 100) {
                    win = 1;
                }
                
                
                
                /* quando jogador ultrapassa x=400 e ainda há mapa à direita, scrolla e trava x em 400 */
                if (player_1->x >= 400 && player_1->control->right && background_x > min_background_x) {
                    background_x -= 7;
                    enemy_2->x -= 7;
                    enemy_3->x -= 7;
                    enemy_4->x -= 7;
                    enemy_5->x -= 7;
                    if (background_x < min_background_x) background_x = min_background_x;
                    player_1->x = 400;
                }
                /* quando jogador vai para a esquerda e há mapa à esquerda, scrolla para direita e trava x em 400 */
                else if (player_1->x <= 400 && player_1->control->left && background_x < 0) {
                    background_x += 7;
                    enemy_2->x += 7;
                    enemy_3->x += 7;
                    enemy_4->x += 7;
                    enemy_5->x += 7;
                    if (background_x > 0) background_x = 0;
                    
                }
                
                al_draw_bitmap(fase, background_x, 0, 0);

                /* Seleciona o sprite correto baseado no estado do player */
                ALLEGRO_BITMAP *current_sprite;
                int flags = 0;
                
                if (player_1->is_crouching) {
                    current_sprite = sprite_crouch;
                } else if (player_1->control->jump) {
                    current_sprite = sprite_jump;
                } else if (player_1->flip) {
                    current_sprite = sprite_idle_left;
                } else {
                    current_sprite = sprite_idle_right;
                }
                
                /* Desenha o sprite na posição do player */
                al_draw_scaled_bitmap(current_sprite,
                                      0, 0,
                                      SPRITE_WIDTH, SPRITE_HEIGHT,
                                      player_1->x - player_1->side_x / 2,
                                      player_1->y - player_1->side_y / 2,
                                      player_1->side_x,
                                      player_1->side_y,
                                      flags);
                

                PhysicsEnemy *pe = find_entry(enemy);
                if (!pe) {
                    pe = create_entry(enemy, enemy->y);
                }

                if (enemy->x - enemy->side_x/2 <= 0){
                    enemy->x = aleat (X_SCREEN, 2 * X_SCREEN);
                }
                else enemy->x -= ENEMY_SPEED;

                if ((int)enemy_2->x - (int)enemy_2->side_x/2 <= 0){
                    enemy_2->x = X_SCREEN+520;
                }
                
                if ((int)enemy_3->x - (int)enemy_3->side_x/2 <= 0){
                    enemy_3->x = aleat (X_SCREEN, 2 * X_SCREEN);
                }

                if ((int)enemy_4->x - (int)enemy_4->side_x/2 <= 0){
                    enemy_4->x = X_SCREEN+750;
                }

                else{
                    enemy_4->side_x -= 1;
                    if (enemy_4->side_x < 1){
                        int hole_cooldown = 60;
                        while (hole_cooldown > 0){
                            hole_cooldown--;
                        }
                        enemy_4->side_x = 70;
                    }
                }

                if ((int)enemy_5->x - (int)enemy_5->side_x/2 <= 0){
                    enemy_5->x = aleat (X_SCREEN, 2 * X_SCREEN);
                }


                enemy_5->y += up_down;
                if ((int)enemy_5->y + (int)enemy_5->side_y/2 >= CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2)){
                    up_down *= -1;
                }
                if ((int)enemy_5->y - (int)enemy_5->side_y/2 <= CHAO_Y-100){
                    up_down *= -1;
                }
                
                if (!pe->in_air){
                    pe->vy = JUMP_VELOCITY_ENM;
                    pe->in_air = true;
                }

                if (collision_cooldown > 0) collision_cooldown--;

                if ((collision_2D(player_1, enemy) || collision_2D(player_1, enemy_2) || collision_2D(player_1, enemy_3) ||  collision_2D(player_1, enemy_4) || collision_2D (player_1, enemy_5)) && collision_cooldown == 0) {
                    player_1->vida--;
                    collision_cooldown = 30;

                    for (int i = 0; i < 3; ++i) {
                        moveJoker(player_1, 1, 0, X_SCREEN, Y_SCREEN);
                    }

                    if (player_1->vida <= 0) {
                        player_1->vida = 0;
                        game_over = 1;
                    }
                }

                updateJokerPhysics(player_1);
                updateEnemyPhysics(enemy);
            }

            if (game_over) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(bigfont, al_map_rgb(255, 0, 0), X_SCREEN / 2, 90, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
                al_draw_textf(font, al_map_rgb(255,255,255), X_SCREEN / 2, 150, ALLEGRO_ALIGN_CENTRE, "Vida: %d", player_1->vida);
                al_draw_text(font, al_map_rgb(255,255,255), X_SCREEN / 2, 200, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
            } else {
                al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "Vida: %hu", player_1->vida);
                al_draw_filled_rectangle(enemy->x-enemy->side_x/2, enemy->y-enemy->side_y/2, enemy->x+enemy->side_x/2, enemy->y+enemy->side_y/2, al_map_rgb(0, 0, 255));
                al_draw_filled_rectangle(enemy_2->x-enemy_2->side_x/2, enemy_2->y-enemy_2->side_y/2, enemy_2->x+enemy_2->side_x/2, enemy_2->y+enemy_2->side_y/2, al_map_rgb(0, 255, 0));
                al_draw_filled_rectangle(enemy_3->x-enemy_3->side_x/2, enemy_3->y-enemy_3->side_y/2, enemy_3->x+enemy_3->side_x/2, enemy_3->y+enemy_3->side_y/2, al_map_rgb(200, 150, 0));
                al_draw_filled_rectangle(enemy_4->x-enemy_4->side_x/2, enemy_4->y-enemy_4->side_y/2, enemy_4->x+enemy_4->side_x/2, enemy_4->y+enemy_4->side_y/2, al_map_rgb(0, 0, 0));
                al_draw_filled_rectangle(enemy_5->x-enemy_5->side_x/2, enemy_5->y-enemy_5->side_y/2, enemy_5->x+enemy_5->side_x/2, enemy_5->y+enemy_5->side_y/2, al_map_rgb(0, 255, 255));
            }

            al_flip_display();
        }
        else if ((event.type == 10) || (event.type == 12)){
            int pressed = (event.type == 10);

            if (event.keyboard.keycode == 1) player_1->control->left  = pressed;
            else if (event.keyboard.keycode == 4) player_1->control->right = pressed;
            else if (event.keyboard.keycode == 23) player_1->control->jump  = pressed;
            else if (event.keyboard.keycode == 19) {
                if (pressed) player_1->control->down = pressed;
                else {
                    player_1->control->down = 0;
                    moveJoker(player_1, -1, 3, X_SCREEN, Y_SCREEN);
                }
            }

            if (game_over && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && !pressed) {
                break;
            }
        }
        else if (event.type == 42) break;
    }

    deleteEnemy (enemy);
    deleteEnemy (enemy_2);
    deleteJoker (player_1);
    al_destroy_bitmap(sprite_idle_right);
    al_destroy_bitmap(sprite_idle_left);
    al_destroy_bitmap(sprite_jump);
    al_destroy_bitmap(sprite_crouch);
    al_destroy_bitmap(joker_sprite_sheet);
    al_destroy_bitmap(fase);
    al_destroy_font(font);
    al_destroy_font(bigfont);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
// ...existing code...