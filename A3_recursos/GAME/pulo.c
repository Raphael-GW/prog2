#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

// Definições de constantes
#define GRAVIDADE 0.5f
#define VELOCIDADE_PULO -15.0f // Valor negativo para mover para cima
#define CHAO_Y 400             // Posição Y do chão na tela

// Variáveis do jogador
float player_x = 100;
float player_y = CHAO_Y;
float player_v_y = 0; // Velocidade vertical
bool esta_no_ar = false;

int main() {
    // ... (Inicialização do Allegro, criação de display e fila de eventos) ...
    al_init();
    al_install_keyboard();
    ALLEGRO_DISPLAY *display = al_create_display(640, 480);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    bool running = true;
    ALLEGRO_EVENT event;

    while (running) {
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Se a tecla ESPAÇO for pressionada e o jogador estiver no chão, pule
            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !esta_no_ar) {
                player_v_y = VELOCIDADE_PULO;
                esta_no_ar = true;
            }
        }

        // Lógica da Física (deve ser atualizada em cada quadro)
        if (esta_no_ar) {
            player_y += player_v_y;
            player_v_y += GRAVIDADE; // Aplica a gravidade (aumenta a velocidade para baixo)

            // Verificação de colisão com o chão
            if (player_y >= CHAO_Y) {
                player_y = CHAO_Y; // Coloca o jogador firmemente no chão
                player_v_y = 0;    // Zera a velocidade vertical
                esta_no_ar = false; // Não está mais no ar
            }
        }

        // Lógica de Desenho
        al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa a tela
        // Desenha o chão
        al_draw_filled_rectangle(0, CHAO_Y + 40, 640, 480, al_map_rgb(100, 100, 100));
        // Desenha o jogador (um círculo)
        al_draw_filled_circle(player_x, player_y, 20, al_map_rgb(255, 255, 0));

        al_flip_display(); // Troca os buffers de exibição
    }

    // ... (Destruição de recursos do Allegro) ...
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
