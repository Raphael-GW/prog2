#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <math.h>
#include "joker.h"
#include "enemy.h"



#define X_SCREEN 1025
#define Y_SCREEN 512
#define CHAO_Y (317 - ALTURA_JOKER/2)

/* Dimensões dos sprites na imagem */
#define SPRITE_WIDTH 30
#define SPRITE_HEIGHT 52

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
    al_init_acodec_addon();

    if (!al_install_keyboard()) {
        fprintf(stderr, "al_install_keyboard failed\n");
        return 1;
    }

    if (!al_install_audio()) {
        fprintf(stderr, "al_install_audio failed\n");
        return 1;
    }

    if (!al_reserve_samples(5)) {
       fprintf(stderr, "al_reserve_samples failed\n");
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
    ALLEGRO_FONT* font = al_load_ttf_font("assets/P5font.ttf", 24, 0);
    ALLEGRO_FONT* bigfont = al_load_ttf_font("assets/P5font.ttf", 48, 0);
    if (!font || !bigfont) {
        fprintf(stderr, "failed to load font(s).\n");
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *fase = al_load_bitmap("assets/stage.png");
    if (!fase) {
        fprintf(stderr, "failed to load bitmap.\n");
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }
    
    ALLEGRO_BITMAP *joker_sprite_sheet = al_load_bitmap("assets/joker_sprite.png");
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

    ALLEGRO_BITMAP *frost_sprite_sheet = al_load_bitmap("assets/Jack_frost.png");
    if (!frost_sprite_sheet) {
        fprintf(stderr, "failed to load frost sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *fence_sprite_sheet = al_load_bitmap("assets/fence.png");
    if (!fence_sprite_sheet) {
        fprintf(stderr, "failed to load frost sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *spike_sprite_sheet = al_load_bitmap("assets/spikes.png");
    if (!spike_sprite_sheet) {
        fprintf(stderr, "failed to load spikes sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *ball_sprite_sheet = al_load_bitmap("assets/ball.png");
    if (!spike_sprite_sheet) {
        fprintf(stderr, "failed to load ball sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *ladder_sprite_sheet = al_load_bitmap("assets/ladder.png");
    if (!spike_sprite_sheet) {
        fprintf(stderr, "failed to load ladder sprite sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_BITMAP *thwomp_sprite_sheet = al_load_bitmap("assets/thwomp.png");
    if (!thwomp_sprite_sheet) {
        fprintf(stderr, "failed to load ladder thwwomp sheet.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_SAMPLE* trilha_sonora = al_load_sample ("assets/trilha.wav");
    if(!trilha_sonora){
        fprintf(stderr, "failed to load audio sample.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_SAMPLE* trilha_pause = al_load_sample ("assets/menu.wav");
    if(!trilha_pause){
        fprintf(stderr, "failed to load audio sample.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    ALLEGRO_SAMPLE* trilha_game_over = al_load_sample ("assets/game_over.wav");
    if(!trilha_game_over){
        fprintf(stderr, "failed to load audio sample.\n");
        al_destroy_bitmap(fase);
        al_destroy_font(font);
        al_destroy_font(bigfont);
        al_destroy_display(disp);
        al_destroy_event_queue(queue);
        al_destroy_timer(timer);
        return 1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Joker* player_1 = createJoker(31, ALTURA_JOKER, 100, CHAO_Y, X_SCREEN, Y_SCREEN);
    if (!player_1) return 1;
    Enemy* enemy = createEnemy(30, ALTURA_ENEMY, X_SCREEN-10, CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy) return 2;
    Enemy* enemy_2 = createEnemy(10, ALTURA_ENEMY+10, aleat(X_SCREEN+30, 2*X_SCREEN), CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2), X_SCREEN, Y_SCREEN);
    if (!enemy_2) return 2;
    Enemy* enemy_3 = createEnemy(20, ALTURA_ENEMY+50, aleat(X_SCREEN+50, 2*X_SCREEN), CHAO_Y-62, X_SCREEN, Y_SCREEN);
    if (!enemy_3) return 2;
    Enemy* enemy_4 = createEnemy(100, 10, aleat(X_SCREEN+90, 2*X_SCREEN), CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2) + 15, X_SCREEN, Y_SCREEN);
    if (!enemy_4) return 2;
    Enemy* enemy_5 = createEnemy(40, ALTURA_ENEMY, aleat(X_SCREEN+130, 2*X_SCREEN), CHAO_Y-37, X_SCREEN, Y_SCREEN);
    if (!enemy_5) return 2;
    Enemy* enemy_6 = createEnemy(50, ALTURA_ENEMY+82, aleat(X_SCREEN+150, 2*X_SCREEN), CHAO_Y-30, X_SCREEN, Y_SCREEN);
    if (!enemy_6) return 2;

    ALLEGRO_EVENT event;
    al_start_timer(timer);

    int collision_cooldown = 0;
    int game_over = 0;
    int game_over_played = 0;

    int menu = 1;
    int bg_w = al_get_bitmap_width(fase);
    int min_background_x = X_SCREEN - bg_w;
    int win = 0;

    int pause = 0;
    int pause_played = 0;
    int pause_just_activated = 0;

    float enemy5_phase = 0.0f;
    const float enemy5_amp = 50.0f; /* amplitude em pixels (ajuste conforme desejar) */
    const float enemy5_speed = 0.08f; /* velocidade angular (ajuste para mais/menos suavidade) */
    int enemy5_base_y = enemy_5->y; /* posição central para oscilar */

    const int ladder_offset_x = -50;    /* conforme pedido: "mesma posição do enemy_6 - 150" */
    const int ladder_w = 24;             /* largura da escada (ajuste se quiser) */
    const int ladder_h = 220;            /* altura da escada (ajuste se quiser) */
    const int ladder_climb_speed = 8;    /* pixels por frame enquanto sobe */
    int w_pressed = 0;                   /* estado da tecla W (1 = pressionada) */
    int ladder_x = enemy_6->x + ladder_offset_x;
    int ladder_y_bottom =  CHAO_Y+(player_1->side_y/2 - ALTURA_ENEMY/2);        /* base da escada alinhada ao chão */
    int ladder_y_top = ladder_y_bottom - ladder_h;

    // variaveis frame
    float frame = 0.f;
    int max_frame = 4;
    
    int current_frame_y = 27;
    int sprite_width = SPRITE_WIDTH;

    float enemy_frame = 0.f;
    float fence_frame = 0.f;
    
    int menu_option = 0;
    int pos_menu_y = 200;

    while(1){
        al_wait_for_event(queue, &event);
        while (menu) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(bigfont, al_map_rgb(255, 255, 255), X_SCREEN / 2, 100, ALLEGRO_ALIGN_CENTRE, "Prog2 - Game");
            al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, 250, ALLEGRO_ALIGN_CENTRE, "INICIAR JOGO");
            al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, 350, ALLEGRO_ALIGN_CENTRE, "SAIR");
            al_draw_rectangle (X_SCREEN/2 - 100, pos_menu_y, X_SCREEN/2 + 100, pos_menu_y + 100, al_map_rgb(255, 0, 0), 3);
            al_flip_display();

            al_wait_for_event(queue, &event);

            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == 19){
                pos_menu_y = 300;
                menu_option = 1;
            }
            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == 23){
                pos_menu_y = 200;
                menu_option = 0;
            }

            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ENTER && menu_option == 0) {
                menu = 0;
                al_play_sample(trilha_sonora, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
            }
            if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ENTER && menu_option == 1) {
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

                if (!pause){
                    update_position(player_1);

                    if (player_1->x >= X_SCREEN - 100) {
                        win = 1;
                    }
                    
                    
                    /* quando jogador ultrapassa x=400 e ainda há mapa à direita, scrolla e trava x em 400 */
                    if (player_1->x >= 400 && player_1->control->right && background_x > min_background_x) {
                        background_x -= 7;
                        enemy->x -= 6;
                        enemy_2->x -= 7;
                        enemy_3->x -= 7;
                        enemy_4->x -= 7;
                        enemy_5->x -= 7;
                        enemy_6->x -= 6;
                        if (background_x < min_background_x) background_x = min_background_x;
                        player_1->x = 400;
                    }
                    /* quando jogador vai para a esquerda e há mapa à esquerda, scrolla para direita e trava x em 400 */
                    else if (player_1->x <= 400 && player_1->control->left && background_x < 0) {
                        background_x += 7;
                        enemy->x += 6;
                        enemy_2->x += 7;
                        enemy_3->x += 7;
                        enemy_4->x += 7;
                        enemy_5->x += 7;
                        enemy_6->x += 6;
                        if (background_x > 0) background_x = 0;
                        
                    }
                    
                    al_draw_bitmap(fase, background_x, 0, 0);

                    /* Atualiza posição da escada com base em enemy_6 */
                    ladder_x = enemy_6->x + ladder_offset_x;
                    ladder_y_bottom = CHAO_Y;
                    ladder_y_top = ladder_y_bottom - ladder_h;

                    /* Verifica se o jogador está "encostado" na escada (colisão AABB) */
                    int px1 = player_1->x - player_1->side_x/2;
                    int py1 = player_1->y - player_1->side_y/2;
                    int px2 = player_1->x + player_1->side_x/2;
                    int py2 = player_1->y + player_1->side_y/2;
                    int lx1 = ladder_x - ladder_w/2;
                    int ly1 = ladder_y_top;
                    int lx2 = ladder_x + ladder_w/2;
                    int ly2 = ladder_y_bottom;

                    int touching_ladder = !(px2 < lx1 || px1 > lx2 || py2 < ly1 || py1 > ly2);

                    /* Se estiver encostado e W for pressionado, sobe ao invés de pular */
                    if (touching_ladder && w_pressed) {
                        /* anula o pulo normal */
                        if (player_1->control) player_1->control->jump = 0;
                        /* sobe suavemente */
                        player_1->y -= ladder_climb_speed;
                        /* não passar do topo da escada */
                        
                    }

                    //atualiza o frame
                    frame += 0.3f;
                    if (frame > max_frame) frame -= max_frame;

                    //atualiza frame inimigo
                    enemy_frame += 0.245f;
                    if (enemy_frame > 7) enemy_frame -= 7;

                    //atualiza frame fence
                    fence_frame += 0.3f;
                    if (fence_frame > 4) fence_frame -= 4;

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

                    enemy5_phase += enemy5_speed;
                    if (enemy5_phase >= 2.0f * 3.14159265f) enemy5_phase -= 2.0f * 3.14159265f;
                    enemy_5->y = enemy5_base_y + (int)(enemy5_amp * sinf(enemy5_phase));
                    
                    if (enemy_6->x - enemy_6->side_x/2 <= 0){
                        enemy_6->x = aleat (X_SCREEN + 150, 2 * X_SCREEN);
                    }
                    else enemy_6->x -= 2;
                    
                    if (!pe->in_air){
                        pe->vy = JUMP_VELOCITY_ENM;
                        pe->in_air = true;
                    }

                    if (collision_cooldown > 0) collision_cooldown--;

                    if ((collision_2D(player_1, enemy) || collision_2D(player_1, enemy_2) || collision_2D(player_1, enemy_3) ||  collision_2D(player_1, enemy_4) || collision_2D (player_1, enemy_5) || collision_2D (player_1, enemy_6)) && collision_cooldown == 0) {
                        player_1->vida--;
                        collision_cooldown = 30;

                        for (int i = 0; i < 3; ++i) {
                            moveJoker(player_1, 1, 0, X_SCREEN, Y_SCREEN);
                        }

                        if (player_1->vida <= 0) {
                            player_1->vida = 0;
                            game_over = 1;
                            game_over_played = 0;
                        }
                    }

                    updateJokerPhysics(player_1);
                    updateEnemyPhysics(enemy);
                }
            }

            if (game_over) {
                if (!game_over_played) {
                    al_stop_samples();
                    al_play_sample(trilha_game_over, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                    game_over_played = 1;
                }
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_text(bigfont, al_map_rgb(255, 0, 0), X_SCREEN / 2, 90, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
                al_draw_textf(font, al_map_rgb(255,255,255), X_SCREEN / 2, 150, ALLEGRO_ALIGN_CENTRE, "Vida: %d", player_1->vida);
                al_draw_text(font, al_map_rgb(255,255,255), X_SCREEN / 2, 200, ALLEGRO_ALIGN_CENTRE, "Pressione ESC para sair");
            } else {
                al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, ALLEGRO_ALIGN_LEFT, "Vida: %hu", player_1->vida);
                al_draw_bitmap_region (ladder_sprite_sheet, 0, 0, ladder_w, ladder_h, ladder_x - ladder_w/2, ladder_y_top, 0);
                al_draw_bitmap_region (thwomp_sprite_sheet, 0, 0, 60, 130, enemy_6->x - enemy_6->side_x/2, enemy_6->y - enemy_6->side_y/2, 0);
                al_draw_bitmap_region (frost_sprite_sheet, 41 * (int)enemy_frame, 17, 40, 70, enemy->x - enemy->side_x/2, enemy->y - enemy->side_y, ALLEGRO_FLIP_HORIZONTAL);
                al_draw_bitmap_region (fence_sprite_sheet, 20 * (int)fence_frame, 4, 20, 60, enemy_2->x - enemy_2->side_x/2, enemy_2->y - enemy_2->side_y/2 - 5, 0);
                al_draw_bitmap_region (spike_sprite_sheet, 35, 13, 35, 100, enemy_3->x - enemy_3->side_x/2 - 3, enemy_3->y - enemy_3->side_y/2 - 3, -1);
                al_draw_filled_rectangle(enemy_4->x-enemy_4->side_x/2, enemy_4->y-enemy_4->side_y/2, enemy_4->x+enemy_4->side_x/2, enemy_4->y+enemy_4->side_y/2, al_map_rgb(0, 0, 0));
                al_draw_bitmap_region (ball_sprite_sheet, 0, 0, 50, 50, enemy_5->x - enemy_5->side_x/2 - 5, enemy_5->y - enemy_5->side_y/2 - 3, 0);
                
                int flip_flag = player_1->flip ? ALLEGRO_FLIP_HORIZONTAL : 0;
                
                al_draw_bitmap_region(joker_sprite_sheet, sprite_width * (int)frame, current_frame_y, sprite_width, SPRITE_HEIGHT, 
                                      player_1->x - player_1->side_x/2, player_1->y - player_1->side_y/2, flip_flag);
                
                /* se estiver em pausa, desenha overlay de pausa */
                if (pause) {
                    if (pause_just_activated) {
                        al_stop_samples();
                        al_play_sample(trilha_pause, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                        pause_played = 1;
                        pause_just_activated = 0;
                    }
                    al_draw_filled_rectangle(X_SCREEN/2 - 100, Y_SCREEN/2 - 50, X_SCREEN/2 + 100, Y_SCREEN/2 + 50, al_map_rgba(0,0,0,180));
                    al_draw_text(bigfont, al_map_rgb(255,255,0), X_SCREEN / 2, Y_SCREEN / 2 - 12, ALLEGRO_ALIGN_CENTRE, "PAUSE");
                } 
                if (!pause && pause_just_activated == 0 && pause_played){
                    // quando sair do pause retorna para a trilha sonora
                    al_stop_samples ();
                    al_play_sample(trilha_sonora, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                    pause_played = 0;
                }
            }

            al_flip_display();
        }
        else if ((event.type == 10) || (event.type == 12)){
            int pressed = (event.type == 10);

            /* tecla ESC alterna pausa (pressionamento) */
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && pressed) {
                /* se estiver em tela de menu ou vitória, keep behavior original */
                if (!menu && !win) {
                    if (!pause) { 
                        // entrando no pause
                        pause = 1;
                        pause_played = 1;
                        pause_just_activated = 1;
                    } else {
                        // saindo do pause
                        pause = 0;
                        pause_played = 1;
                        pause_just_activated = 0;
                    }
                }
            }

            /* tecla W para subir escada (mantém estado enquanto pressionada) */
            if (event.keyboard.keycode == ALLEGRO_KEY_W) {
                w_pressed = pressed;
            }


            if (event.keyboard.keycode == 1) {
                player_1->control->left  = pressed;
                if (pressed){
                    sprite_width = 31;
                    current_frame_y = 189;
                    max_frame = 8;
                }
                else{
                    current_frame_y = 27;
                    sprite_width = SPRITE_WIDTH;
                    max_frame = 4;
                }
            }
            else if (event.keyboard.keycode == 4) {
                player_1->control->right = pressed;
                if (pressed){
                    sprite_width = 31;
                    current_frame_y = 189;
                    max_frame = 8;
                }
                else{
                    current_frame_y = 27;
                    sprite_width = SPRITE_WIDTH;
                    max_frame = 4;
                }
            }
            else if (event.keyboard.keycode == 23) {
                player_1->control->jump  = pressed;
                if (pressed){
                    sprite_width = 31;
                    current_frame_y = 106;
                    max_frame = 8;
                }
                else{
                    current_frame_y = 27;
                    sprite_width = SPRITE_WIDTH;
                    max_frame = 4;
                }
            }
            else if (event.keyboard.keycode == 19) {
                if (pressed){
                    player_1->control->down = pressed;
                    sprite_width = 31;
                    current_frame_y = 126;
                    max_frame = 1;
                }
                else {
                    player_1->control->down = 0;
                    moveJoker(player_1, -1, 3, X_SCREEN, Y_SCREEN);
                    current_frame_y = 27;
                    sprite_width = SPRITE_WIDTH;
                    max_frame = 4;
                }
            }

            if (game_over && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && !pressed) {
                al_stop_samples();
                break;
            }
        }
        else if (event.type == 42) break;
    }

    deleteEnemy (enemy);
    deleteEnemy (enemy_2);
    deleteEnemy (enemy_3);
    deleteEnemy (enemy_4);
    deleteEnemy (enemy_5);
    deleteEnemy (enemy_6);
    deleteJoker (player_1);
    al_destroy_bitmap(joker_sprite_sheet);
    al_destroy_bitmap(frost_sprite_sheet);
    al_destroy_bitmap(fase);
    al_destroy_font(font);
    al_destroy_font(bigfont);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
// ...existing code...