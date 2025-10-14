#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

//Estados de Jogo
typedef enum {
    ESTADO_INICIAL,
    ESTADO_CONTEXTUALIZACAO,
    ESTADO_ALERTA,
    ESTADO_INTERROGATORIO,
    ESTADO_MAPA_BRASIL,
    ESTADO_MAPA_PRISAO,
    ESTADO_MAPA_EXILIO
} EstadoJogo;

//Personagem
typedef struct {
    float x;
    float y;
    int direcao;
    ALLEGRO_BITMAP* parado;
    ALLEGRO_BITMAP* andando_cima;
    ALLEGRO_BITMAP* andando_baixo;
    ALLEGRO_BITMAP* andando_esq;
    ALLEGRO_BITMAP* andando_dir;
} Personagem;

//Funções
int inicializar_allegro(void);
int carrega_imagens(void);
void destruir_recursos(void);
void game_loop(void);

void desenha_tela_inicial(void);
EstadoJogo logica_tela_inicial(ALLEGRO_EVENT* ev);
EstadoJogo logica_transicao_por_toque(EstadoJogo estado_atual, ALLEGRO_EVENT* ev);
EstadoJogo logica_mapa_prisao(ALLEGRO_EVENT* ev);
EstadoJogo logica_tela_interrogatorio(ALLEGRO_EVENT* ev);

void setup_personagem(Personagem* p);
void desenha_personagem(Personagem* p);
void atualiza_personagem(Personagem* p, ALLEGRO_EVENT* ev);

//Tela de fundo
const int LARGURA = 1536;
const int ALTURA = 1024;

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE* event_queue = NULL;

EstadoJogo estado_atual = ESTADO_INICIAL;
Personagem jogador;

//Mapas
ALLEGRO_BITMAP* fundo_inicial = NULL;
ALLEGRO_BITMAP* fundo_contextualizacao = NULL;
ALLEGRO_BITMAP* fundo_alerta = NULL;
ALLEGRO_BITMAP* fundo_interrogatorio = NULL;
ALLEGRO_BITMAP* fundo_mapa_brasil = NULL;
ALLEGRO_BITMAP* fundo_mapa_prisao = NULL;
ALLEGRO_BITMAP* fundo_mapa_exilio = NULL;

//Botao de jogar
const int BTN_INICIO_X = 482;
const int BTN_INICIO_Y = 776;
const int BTN_INICIO_LARGURA = 444;
const int BTN_INICIO_ALTURA = 127;

//Mini game da prisão
float quadrado_x = 100, quadrado_y = 100;
const float bolinha_x = 400, bolinha_y = 300;
bool arrastando_quadrado = false;

//Imagens
int carrega_imagens() {
    fundo_inicial = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/menu_inicial.png");
    fundo_contextualizacao = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/context_background.png.png");
    fundo_alerta = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/alert_background.png.png");
    fundo_interrogatorio = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/mapa_principal.png.png");
    fundo_mapa_brasil = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/mapa_brasil.png.png");
    fundo_mapa_prisao = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/prisao_map.png.png");
    fundo_mapa_exilio = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/exilio_map.png.png");

    jogador.parado = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/player_sprite.png.png");
    jogador.andando_cima = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/player_sprite_cima.png.png");
    jogador.andando_baixo = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/player_sprit_baixo.png.png");
    jogador.andando_esq = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/player_sprite_esquerda.png.png");
    jogador.andando_dir = al_load_bitmap("C:/Users/annya/Desktop/Brasil_aod/x64/Debug/player_sprite_direita.png.png");

    return 1;
}
//Detruição bitmaps personagem
void destruir_recursos() {

    if (fundo_inicial) al_destroy_bitmap(fundo_inicial);
    if (fundo_contextualizacao) al_destroy_bitmap(fundo_contextualizacao);
    if (fundo_alerta) al_destroy_bitmap(fundo_alerta);
    if (fundo_interrogatorio) al_destroy_bitmap(fundo_interrogatorio);
    if (fundo_mapa_brasil) al_destroy_bitmap(fundo_mapa_brasil);
    if (fundo_mapa_prisao) al_destroy_bitmap(fundo_mapa_prisao);
    if (fundo_mapa_exilio) al_destroy_bitmap(fundo_mapa_exilio);

    if (jogador.parado) al_destroy_bitmap(jogador.parado);
    if (jogador.andando_cima) al_destroy_bitmap(jogador.andando_cima);
    if (jogador.andando_baixo) al_destroy_bitmap(jogador.andando_baixo);
    if (jogador.andando_esq) al_destroy_bitmap(jogador.andando_esq);
    if (jogador.andando_dir) al_destroy_bitmap(jogador.andando_dir);

    if (display) al_destroy_display(display);
    if (event_queue) al_destroy_event_queue(event_queue);
}

//Função para mudar o mapas
void desenha_tela_inicial(void) {
    al_draw_bitmap(fundo_inicial, 0, 0, 0);
}
//Para contextualização e alerta (clique)
EstadoJogo logica_tela_inicial(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        int mx = ev->mouse.x;
        int my = ev->mouse.y;

        if (mx >= BTN_INICIO_X && mx <= BTN_INICIO_X + BTN_INICIO_LARGURA &&
            my >= BTN_INICIO_Y && my <= BTN_INICIO_Y + BTN_INICIO_ALTURA)
        {
            return ESTADO_CONTEXTUALIZACAO;
        }
    }
    return ESTADO_INICIAL;
}

//Para interrogatório tbm
EstadoJogo logica_transicao_por_toque(EstadoJogo estado_atual, ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (estado_atual == ESTADO_CONTEXTUALIZACAO) {
            return ESTADO_ALERTA;
        }
        else if (estado_atual == ESTADO_ALERTA) {
            return ESTADO_INTERROGATORIO;
        }
    }
    return estado_atual;
}

//Desenho contextualização e alerta
void desenha_tela_contextualizacao(void) { al_draw_bitmap(fundo_contextualizacao, 0, 0, 0); }
void desenha_tela_alerta(void) { al_draw_bitmap(fundo_alerta, 0, 0, 0); }

//Desenho interrogatório
void desenha_tela_interrogatorio(void) {
    al_draw_bitmap(fundo_interrogatorio, 0, 0, 0);
}

EstadoJogo logica_tela_interrogatorio(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        int mx = ev->mouse.x;
        if (mx < LARGURA / 2) {
            setup_personagem(&jogador);
            return ESTADO_MAPA_BRASIL;
        }
        else {
            return ESTADO_MAPA_PRISAO;
        }
    }
    return ESTADO_INTERROGATORIO;
}

//Funções da prisão
void desenha_mapa_prisao(void) {
    al_draw_bitmap(fundo_mapa_prisao, 0, 0, 0);

    //Desenho do quadrado e da bola
    al_draw_filled_circle(bolinha_x, bolinha_y, 25, al_map_rgb(255, 0, 0));
    al_draw_filled_rectangle(quadrado_x - 20, quadrado_y - 20, quadrado_x + 20, quadrado_y + 20, al_map_rgb(0, 0, 255)); // Quadrado
}

EstadoJogo logica_mapa_prisao(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        //Verificação
        if (ev->mouse.x >= quadrado_x - 20 && ev->mouse.x <= quadrado_x + 20 &&
            ev->mouse.y >= quadrado_y - 20 && ev->mouse.y <= quadrado_y + 20) {
            arrastando_quadrado = true;
        }
    }
    //Movimento do quadrado
    else if (ev->type == ALLEGRO_EVENT_MOUSE_AXES && arrastando_quadrado) {
        quadrado_x = ev->mouse.x;
        quadrado_y = ev->mouse.y;
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (arrastando_quadrado) {
            arrastando_quadrado = false;

            //Parte da colisão
            float dx = quadrado_x - bolinha_x;
            float dy = quadrado_y - bolinha_y;
            float distancia_minima = 25.0 + 20.0;

            if (dx * dx + dy * dy < distancia_minima * distancia_minima) {
                setup_personagem(&jogador);
                return ESTADO_MAPA_EXILIO;
            }
        }
    }
    return ESTADO_MAPA_PRISAO;
}

//Movimentação do personagem
void setup_personagem(Personagem* p) {
    p->x = LARGURA / 2;
    p->y = ALTURA / 2;
    p->direcao = 0;
}
void desenha_personagem(Personagem* p) {
    ALLEGRO_BITMAP* sprite_atual = NULL;

    switch (p->direcao) {
    case 1: sprite_atual = p->andando_cima; break;
    case 2: sprite_atual = p->andando_baixo; break;
    case 3: sprite_atual = p->andando_esq; break;
    case 4: sprite_atual = p->andando_dir; break;
    case 0:
    default: sprite_atual = p->parado; break;
    }

    if (sprite_atual) {
        float sprite_w = al_get_bitmap_width(sprite_atual);
        float sprite_h = al_get_bitmap_height(sprite_atual);
        al_draw_bitmap(sprite_atual, p->x - sprite_w / 2, p->y - sprite_h / 2, 0);
    }
}

void atualiza_personagem(Personagem* p, ALLEGRO_EVENT* ev) {
    float velocidade = 3.0;

    //Leitura do teclado
    if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (ev->keyboard.keycode == ALLEGRO_KEY_W) p->direcao = 1;
        else if (ev->keyboard.keycode == ALLEGRO_KEY_S) p->direcao = 2;
        else if (ev->keyboard.keycode == ALLEGRO_KEY_A) p->direcao = 3;
        else if (ev->keyboard.keycode == ALLEGRO_KEY_D) p->direcao = 4;
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_UP) {
        p->direcao = 0;
    }
    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (p->direcao == 1) p->y -= velocidade;
        else if (p->direcao == 2) p->y += velocidade;
        else if (p->direcao == 3) p->x -= velocidade;
        else if (p->direcao == 4) p->x += velocidade;

        //Limites
        if (p->x < 0) p->x = 0;
        if (p->x > LARGURA) p->x = LARGURA;
        if (p->y < 0) p->y = 0;
        if (p->y > ALTURA) p->y = ALTURA;
    }
}

int inicializar_allegro() {
    
    if (!al_init() || !al_init_image_addon() || !al_init_primitives_addon() ||
        !al_install_mouse() || !al_install_keyboard()) {
        fprintf(stderr, "Não inicializou o allegro");
        return 0;
    }

    display = al_create_display(LARGURA, ALTURA);
    event_queue = al_create_event_queue();

    if (!display || !event_queue) {
        fprintf(stderr, "Não criou o display e a fila");
        return 0;
    }
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    return 1;
}


void game_loop() {
    bool done = false;
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    bool redraw = true;
    //Eventos
    while (!done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        }

        EstadoJogo proximo_estado = estado_atual;

        switch (estado_atual) {
        case ESTADO_INICIAL:
            proximo_estado = logica_tela_inicial(&ev);
            break;
        case ESTADO_CONTEXTUALIZACAO:
        case ESTADO_ALERTA:
            proximo_estado = logica_transicao_por_toque(estado_atual, &ev);
            break;
        case ESTADO_INTERROGATORIO:
            proximo_estado = logica_tela_interrogatorio(&ev);
            break;
        case ESTADO_MAPA_PRISAO:
            proximo_estado = logica_mapa_prisao(&ev);
            break;
        case ESTADO_MAPA_BRASIL:
        case ESTADO_MAPA_EXILIO:
            atualiza_personagem(&jogador, &ev);
            break;
        }

        if (proximo_estado != estado_atual) {
            estado_atual = proximo_estado;
            redraw = true;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            //Velocidade do personagem
            if (estado_atual == ESTADO_MAPA_BRASIL || estado_atual == ESTADO_MAPA_EXILIO) {
                atualiza_personagem(&jogador, &ev);
            }
        }

        //Desenho
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            switch (estado_atual) {
            case ESTADO_INICIAL: desenha_tela_inicial(); break;
            case ESTADO_CONTEXTUALIZACAO: desenha_tela_contextualizacao(); break;
            case ESTADO_ALERTA: desenha_tela_alerta(); break;
            case ESTADO_INTERROGATORIO: desenha_tela_interrogatorio(); break;

            case ESTADO_MAPA_BRASIL:
                al_draw_bitmap(fundo_mapa_brasil, 0, 0, 0);
                desenha_personagem(&jogador);
                break;

            case ESTADO_MAPA_PRISAO: desenha_mapa_prisao(); break;

            case ESTADO_MAPA_EXILIO:
                al_draw_bitmap(fundo_mapa_exilio, 0, 0, 0);
                desenha_personagem(&jogador);
                break;
            }

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
}

int main() {
    if (inicializar_allegro()) {
        if (carrega_imagens()) {
            game_loop();
        }
        else {
            fprintf(stderr, "Não funcionou a função principal");
        }
    }

    destruir_recursos();
    return 0;
}