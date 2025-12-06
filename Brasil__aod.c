#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    ESTADO_INICIAL,
    ESTADO_COMO_JOGAR,
    ESTADO_CONTEXTUALIZACAO,
    ESTADO_ALERTA,
    ESTADO_INTERROGATORIO,
    ESTADO_INTERROGATORIO_2,
    ESTADO_MAPA_BRASIL,
    ESTADO_MAPA_PRISAO,
    ESTADO_MINIGAME1,    // Decifrar
    ESTADO_MINIGAME2,    // Labirinto 
    ESTADO_MAPA_EXILIO,
    ESTADO_MINIGAME3,    // Transmissor 
    ESTADO_MINIGAME4,    // Espalhar msg 
    ESTADO_MINIGAME5,    // Coletar msg 
    ESTADO_MINIGAME6,
    ESTADO_TELA_VOCE_GANHOU
} EstadoJogo;

// Personagem
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

// Constantes tela
const int LARGURA = 1536;
const int ALTURA = 1024;

// Globais
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE* event_queue = NULL;
ALLEGRO_TIMER* timer_global = NULL;
ALLEGRO_FONT* fonte_builtin = NULL;

EstadoJogo estado_atual = ESTADO_INICIAL;
Personagem jogador;

// Mapas e etc
ALLEGRO_BITMAP* fundo_inicial = NULL;
ALLEGRO_BITMAP* fundo_como_jogar = NULL;
ALLEGRO_BITMAP* fundo_contextualizacao = NULL;
ALLEGRO_BITMAP* fundo_alerta = NULL;
ALLEGRO_BITMAP* fundo_interrogatorio = NULL;
ALLEGRO_BITMAP* fundo_interrogatorio_2 = NULL;
ALLEGRO_BITMAP* fundo_mapa_brasil = NULL;
ALLEGRO_BITMAP* fundo_mapa_prisao = NULL;
ALLEGRO_BITMAP* fundo_mapa_exilio = NULL;
ALLEGRO_BITMAP* fundo_tela_voce_ganhou = NULL;
ALLEGRO_BITMAP* mg2_guarda_img = NULL;
ALLEGRO_BITMAP* mg2_jogador_img = NULL;
ALLEGRO_BITMAP* mg3_peca_1_img = NULL;
ALLEGRO_BITMAP* mg3_peca_2_img = NULL;
ALLEGRO_BITMAP* mg3_peca_3_img = NULL;
ALLEGRO_BITMAP* mg3_peca_4_img = NULL;
ALLEGRO_BITMAP* mg3_peca_5_img = NULL;
ALLEGRO_BITMAP* mg3_peca_6_img = NULL;
ALLEGRO_BITMAP* mg5_jogador_img = NULL;   
ALLEGRO_BITMAP* mg5_perseguidor_img = NULL; 
ALLEGRO_BITMAP* mg5_coletavel_img = NULL;
ALLEGRO_BITMAP* mg6_heli_img = NULL;

const int BTN_INICIO_X = 503;
const int BTN_INICIO_Y = 689;
const int BTN_INICIO_LARGURA = 444;
const int BTN_INICIO_ALTURA = 127;


// Botão de como jogar
const int BTN_INSTRUCOES_X = 600;   
const int BTN_INSTRUCOES_Y = 874;  
const int BTN_INSTRUCOES_LARGURA = 282; 
const int BTN_INSTRUCOES_ALTURA = 75;

// Botões do interrotatorio 2
const int BTN_BRASIL_X = 276;
const int BTN_BRASIL_Y = 663;
const int BTN_BRASIL_LARGURA = 980;
const int BTN_BRASIL_ALTURA = 75;

// Botões da prisão
const int BTN_PRISAO_1_X = 276;
const int BTN_PRISAO_1_Y = 400;
const int BTN_PRISAO_1_LARGURA = 980;
const int BTN_PRISAO_1_ALTURA = 75;

const int BTN_PRISAO_2_X = 276;
const int BTN_PRISAO_2_Y = 525;
const int BTN_PRISAO_2_LARGURA = 980;
const int BTN_PRISAO_2_ALTURA = 75;

const int BTN_PRISAO_3_X = 276;
const int BTN_PRISAO_3_Y = 805;
const int BTN_PRISAO_3_LARGURA = 980;
const int BTN_PRISAO_3_ALTURA = 75;

// botões interrogatorio 2
const int BTN2_CERTO_X = 174;
const int BTN2_CERTO_Y = 365;
const int BTN2_CERTO_LARGURA = 1187;
const int BTN2_CERTO_ALTURA = 90;

const int BTN2_ERRADO_1_X = 174;
const int BTN2_ERRADO_1_Y = 494;
const int BTN2_ERRADO_1_LARGURA = 1187;
const int BTN2_ERRADO_1_ALTURA = 90;

const int BTN2_ERRADO_2_X = 174;
const int BTN2_ERRADO_2_Y = 645;
const int BTN2_ERRADO_2_LARGURA = 1187;
const int BTN2_ERRADO_2_ALTURA = 90;

const int BTN2_ERRADO_3_X = 174;
const int BTN2_ERRADO_3_Y = 795;
const int BTN2_ERRADO_3_LARGURA = 1187;
const int BTN2_ERRADO_3_ALTURA = 90;


// prisão bolinha e quadrado
float quadrado_x = 100, quadrado_y = 100;
const float bolinha_x = 400, bolinha_y = 300;
bool arrastando_quadrado = false;

// -------------------- Variáveis MiniGame1 --------------------
int mg1_tempo_restante = 60;
char mg1_entrada[64];
int mg1_pos = 0;
bool mg1_venceu = false;
bool mg1_perdeu = false;
bool mg1_tentativa_errada = false;
const char* mg1_mensagem_codificada = "Prafhen";
const char* mg1_mensagem_correta = "Censura";
const char* mg1_dica = "DICA: Utilize ROT13 e lembre-se, é algo muito utilizado por governos totalitários.";

// -------------------- Variáveis MiniGame2 --------------------
#define MG2_TILE 40
#define MG2_VELOC 4
#define MG2_NUM_GUARDAS 3
#define MG2_TEMPO_MAX 30.0f

int mg2_labirinto[15][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct {
    float x, y;
    float velX, velY;
    int tempoTroca;
} Guarda;

Guarda mg2_guardas[MG2_NUM_GUARDAS];
float mg2_jogX, mg2_jogY;
float mg2_tempo;
bool mg2_fim = false, mg2_venceu = false, mg2_podeTentar = false;
bool mg2_teclas[4] = { false, false, false, false };

// -------------------- Variaveis MiniGame3 --------------------
typedef struct {
    float x, y;       
    float alvoX, alvoY; 
    bool correto;
    bool arrastando;
    float offsetX, offsetY;
    int tipo; 
} PecaMG3;

#define MG3_TEMPO_LIMITE 30.0f
PecaMG3 mg3_pecas[6];
float mg3_tempo;
bool mg3_ganhou = false, mg3_perdeu = false;
bool mg3_ativado = false; 
const float TRG3_X = 300.0f;
const float TRG3_Y = 850.0f;
const float TRG3_RAIO = 48.0f;
bool mg3_completed = false;

// ------------------ Variaveis MiniGame4 --------------------
typedef struct {
    const char* nome;
    const char* descricao;
    int alcance;
    int risco;
} Pais;

#define MG4_RODADAS 4
int mg4_rodada = 0;
int mg4_alcance = 0, mg4_risco = 0;
bool mg4_mostrandoFeedback = false;
double mg4_feedbackTimer = 0.0;
char mg4_feedbackTexto[256] = "";
int mg4_opcoes[MG4_RODADAS][3];
bool mg4_venceu = false, mg4_finalizado = false;
bool mg4_ativado = false;
const float TRG4_X = 1386.0f;
const float TRG4_Y = 300.0f;
const float TRG4_RAIO = 48.0f;

Pais mg4_pool[] = {
    {"França",  "Imprensa influente na Europa, publica reportagens.", 20, 5},
    {"Suécia",  "Meios acolhedores a exilados — ampla difusão.", 25, -3},
    {"Chile",   "Governo autoritário local - difícil divulgação.", 5, 12},
    {"Portugal","Comunidade brasileira forte, alcance moderado.", 15, 4},
    {"EUA",     "Grande alcance mediático, risco diplomático.", 30, 8},
    {"México",  "Rede de exilados e jornais alternativos.", 25, 6},
    {"Holanda", "Tradição de liberdade de imprensa.", 17, 3},
    {"Alemanha","Jornais europeus dispostos a investigar.", 22, 6},
    {"Brasil (clandestino)", "Publicação interna, altamente arriscada.", 35, 25}
};
int mg4_poolSize = sizeof(mg4_pool) / sizeof(mg4_pool[0]);

// ------------------- Veriaveis MiniGame5 --------------------
#define MG5_BASE_W 800.0f
#define MG5_BASE_H 600.0f
#define MG5_TILE 40
#define MG5_COLS ((int)(MG5_BASE_W / MG5_TILE))   // 20
#define MG5_ROWS ((int)(MG5_BASE_H / MG5_TILE))   // 15
#define MG5_NUM_ITEMS 5
#define MG5_NUM_GUARDS 4
#define MG5_START_TIME 60.0f
#define MG5_PENALTY 5.0f

static int mg5_map[MG5_ROWS][MG5_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,0,0,1},
    {1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,0,1,0,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct { float x, y; int gx, gy; } MG5_Player;
typedef struct { int gx, gy; float x, y; int target_gx, target_gy; bool moving; } MG5_Guard;
typedef struct { int gx, gy; bool taken; } MG5_Item;

MG5_Player mg5_player;
MG5_Player mg5_player_start;
MG5_Guard mg5_guards[MG5_NUM_GUARDS];
MG5_Item mg5_items[MG5_NUM_ITEMS];
float mg5_remaining = MG5_START_TIME;
bool mg5_game_over = false;
bool mg5_win = false;
bool mg5_can_retry = false;
float mg5_scale_x = 1.0f, mg5_scale_y = 1.0f;
int mg5_exit_gx, mg5_exit_gy;
bool mg5_initialized = false;

const float TRG5_X = 146.0f;
const float TRG5_Y = 886.0f;
const float TRG5_RAIO = 64.0f;

// ------------------- Variaveis MiniGame6 ------------------
#define MG6_BASE_W 800.0f
#define MG6_BASE_H 600.0f

float mg6_barraLargura = 300.0f;
float mg6_barraAltura = 30.0f;
float mg6_margem = 40.0f;
float mg6_barraX, mg6_barraY;
float mg6_indicadorX, mg6_velocidade;
float mg6_zonaVerdeInicio, mg6_zonaVerdeFim;
float mg6_heliX, mg6_heliY;
float mg6_personagemX, mg6_personagemY, mg6_chaoY, mg6_personagemAltura;
bool mg6_jogando = false, mg6_ganhou = false, mg6_perdeu = false;
bool mg6_finished = false;
double mg6_finish_timer = 0.0;
float mg6_scale_x = 1.0f, mg6_scale_y = 1.0f;
bool mg6_initialized = false;


int inicializar_allegro(void);
int carrega_imagens(void);
void destruir_recursos(void);
void game_loop(void);

// desenhar
void desenha_tela_inicial(void);
void desenha_tela_como_jogar(void);
void desenha_tela_contextualizacao(void);
void desenha_tela_alerta(void);
void desenha_tela_interrogatorio(void);
void desenha_tela_interrogatorio_2(void);
void desenha_mapa_prisao(void);
void desenha_tela_voce_ganhou(void);

// lógicas
EstadoJogo logica_tela_inicial(ALLEGRO_EVENT* ev);
EstadoJogo logica_tela_como_jogar(ALLEGRO_EVENT* ev);
EstadoJogo logica_transicao_por_toque(EstadoJogo estado_atual, ALLEGRO_EVENT* ev);
EstadoJogo logica_tela_interrogatorio(ALLEGRO_EVENT* ev);
EstadoJogo logica_tela_interrogatorio_2(ALLEGRO_EVENT* ev);
EstadoJogo logica_mapa_prisao(ALLEGRO_EVENT* ev);
EstadoJogo logica_tela_voce_ganhou(ALLEGRO_EVENT* ev);

// personagem
void setup_personagem(Personagem* p);
void desenha_personagem(Personagem* p);
void atualiza_personagem(Personagem* p, ALLEGRO_EVENT* ev);

// MiniGame 1 e 2
void mg1_reset(void);
void mg2_reset(void);
EstadoJogo mg1_logica(ALLEGRO_EVENT* ev);
void mg1_draw(void);
EstadoJogo mg2_logica(ALLEGRO_EVENT* ev);
void mg2_draw(void);

// MiniGame 3 e 4
void mg3_reset(void);
EstadoJogo mg3_logica(ALLEGRO_EVENT* ev);
void mg3_draw(void);

void mg4_reset(void);
EstadoJogo mg4_logica(ALLEGRO_EVENT* ev);
void mg4_draw(void);

// MiniGame 5 e 6
void mg5_reset(void);
EstadoJogo mg5_logica(ALLEGRO_EVENT* ev);
void mg5_draw(void);

void mg6_reset(void);
EstadoJogo mg6_logica(ALLEGRO_EVENT* ev);
void mg6_draw(void);

bool mg2_colisao(int linha, int coluna) {
    if (linha < 0 || linha >= 15 || coluna < 0 || coluna >= 20) return true;
    return mg2_labirinto[linha][coluna] == 1;
}

// -------------------- Carregar imagens --------------------

int carrega_imagens() {

    fundo_inicial = al_load_bitmap("./Imagens/menu_inicial.png");
    mg2_guarda_img = al_load_bitmap("./Imagens/policia_bolota.png");
    mg2_jogador_img = al_load_bitmap("./Imagens/personagem_bolota.png");
    fundo_tela_voce_ganhou = al_load_bitmap("./Imagens/tela_voce_ganhou.png.png");
    fundo_como_jogar = al_load_bitmap("./Imagens/tela_como_jogar.png.png");
    fundo_contextualizacao = al_load_bitmap("./Imagens/context_background.png.png");
    fundo_alerta = al_load_bitmap("./Imagens/alert_background.png.png");
    fundo_interrogatorio = al_load_bitmap("./Imagens/mapa_principal.png.png");
    fundo_interrogatorio_2 = al_load_bitmap("./Imagens/interrogatorio_2.png.png");
    fundo_mapa_brasil = al_load_bitmap("./Imagens/mapa_brasil.png.png");
    fundo_mapa_prisao = al_load_bitmap("./Imagens/prisao_map.png.png");
    fundo_mapa_exilio = al_load_bitmap("./Imagens/exilio_map.png.png");
    jogador.parado = al_load_bitmap("./Imagens/player_sprite.png.png");
    jogador.andando_cima = al_load_bitmap("./Imagens/player_sprite_cima.png.png");
    jogador.andando_baixo = al_load_bitmap("./Imagens/personagem.png.png");
    jogador.andando_esq = al_load_bitmap("./Imagens/player_sprite_esquerda.png.png");
    jogador.andando_dir = al_load_bitmap("./Imagens/player_sprite_direita.png.png");
    mg3_peca_1_img = al_load_bitmap("./Imagens/peca1.png");
    mg3_peca_2_img = al_load_bitmap("./Imagens/peca2.png");
    mg3_peca_3_img = al_load_bitmap("./Imagens/peca3.png");
    mg3_peca_4_img = al_load_bitmap("./Imagens/peca4.png");
    mg3_peca_5_img = al_load_bitmap("./Imagens/peca5.png");
    mg3_peca_6_img = al_load_bitmap("./Imagens/peca6.png");
    mg5_coletavel_img = al_load_bitmap("./Imagens/carta.png");
    mg5_jogador_img = al_load_bitmap("./Imagens/personagem_bolota.png");
    mg5_perseguidor_img = al_load_bitmap("./Imagens/policia_bolota.png");
    mg6_heli_img = al_load_bitmap("./Imagens/heli.png");

    return 1;

}

void destruir_recursos() {
    if (fundo_inicial) al_destroy_bitmap(fundo_inicial);
    if (mg2_guarda_img) al_destroy_bitmap(mg2_guarda_img);
    if (mg2_jogador_img) al_destroy_bitmap(mg2_jogador_img);
    if (fundo_tela_voce_ganhou) al_destroy_bitmap(fundo_tela_voce_ganhou);
    if (fundo_como_jogar) al_destroy_bitmap(fundo_como_jogar);
    if (fundo_contextualizacao) al_destroy_bitmap(fundo_contextualizacao);
    if (fundo_alerta) al_destroy_bitmap(fundo_alerta);
    if (fundo_interrogatorio) al_destroy_bitmap(fundo_interrogatorio);;
    if (fundo_interrogatorio_2) al_destroy_bitmap(fundo_interrogatorio_2);
    if (fundo_mapa_brasil) al_destroy_bitmap(fundo_mapa_brasil);
    if (fundo_mapa_prisao) al_destroy_bitmap(fundo_mapa_prisao);
    if (fundo_mapa_exilio) al_destroy_bitmap(fundo_mapa_exilio);
    if (jogador.parado) al_destroy_bitmap(jogador.parado);
    if (jogador.andando_cima) al_destroy_bitmap(jogador.andando_cima);
    if (jogador.andando_baixo) al_destroy_bitmap(jogador.andando_baixo);
    if (jogador.andando_esq) al_destroy_bitmap(jogador.andando_esq);
    if (jogador.andando_dir) al_destroy_bitmap(jogador.andando_dir);
    if (fonte_builtin) al_destroy_font(fonte_builtin);
    if (timer_global) al_destroy_timer(timer_global);
    if (event_queue) al_destroy_event_queue(event_queue);
    if (display) al_destroy_display(display);
    if (mg3_peca_1_img) al_destroy_bitmap(mg3_peca_1_img);
    if (mg3_peca_2_img) al_destroy_bitmap(mg3_peca_2_img);
    if (mg3_peca_3_img) al_destroy_bitmap(mg3_peca_3_img);
    if (mg3_peca_4_img) al_destroy_bitmap(mg3_peca_4_img);
    if (mg3_peca_5_img) al_destroy_bitmap(mg3_peca_5_img);
    if (mg3_peca_6_img) al_destroy_bitmap(mg3_peca_6_img);
}

// desenho
void desenha_tela_inicial(void) {
    if (fundo_inicial) al_draw_bitmap(fundo_inicial, 0, 0, 0);
    else al_clear_to_color(al_map_rgb(10, 10, 20));
}
void desenha_tela_voce_ganhou(void) {
    if (fundo_tela_voce_ganhou) al_draw_bitmap(fundo_tela_voce_ganhou, 0, 0, 0);
    else al_clear_to_color(al_map_rgb(15, 15, 30));
}

void desenha_tela_contextualizacao(void) {
    if (fundo_contextualizacao) al_draw_bitmap(fundo_contextualizacao, 0, 0, 0);
    else al_clear_to_color(al_map_rgb(15, 15, 30));
}

void desenha_tela_alerta(void) {
    if (fundo_alerta) al_draw_bitmap(fundo_alerta, 0, 0, 0);
    else al_clear_to_color(al_map_rgb(20, 20, 40));
}

void desenha_tela_interrogatorio(void) {
    if (fundo_interrogatorio) al_draw_bitmap(fundo_interrogatorio, 0, 0, 0);
    else {
        al_clear_to_color(al_map_rgb(25, 25, 45));
        al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, 200, ALLEGRO_ALIGN_CENTRE,
            "Interrogatorio - clique esquerda para salvar-se, direita para ir à prisão");
    }
}

void desenha_tela_interrogatorio_2(void) {
    al_draw_bitmap(fundo_interrogatorio_2, 0, 0, 0);
}

// prisão
void desenha_mapa_prisao(void) {
    if (fundo_mapa_prisao) al_draw_bitmap(fundo_mapa_prisao, 0, 0, 0);
    else al_clear_to_color(al_map_rgb(30, 10, 10));

    al_draw_filled_circle(bolinha_x, bolinha_y, 25, al_map_rgb(255, 0, 0));
    al_draw_filled_rectangle(quadrado_x - 20, quadrado_y - 20, quadrado_x + 20, quadrado_y + 20, al_map_rgb(0, 0, 255));
}

// lógica telas
EstadoJogo logica_tela_inicial(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        int mx = ev->mouse.x, my = ev->mouse.y;

        if (mx >= BTN_INICIO_X && mx <= BTN_INICIO_X + BTN_INICIO_LARGURA &&
            my >= BTN_INICIO_Y && my <= BTN_INICIO_Y + BTN_INICIO_ALTURA) {
            return ESTADO_CONTEXTUALIZACAO;
        }

        if (mx >= BTN_INSTRUCOES_X && mx <= BTN_INSTRUCOES_X + BTN_INSTRUCOES_LARGURA &&
            my >= BTN_INSTRUCOES_Y && my <= BTN_INSTRUCOES_Y + BTN_INSTRUCOES_ALTURA) {
            return ESTADO_COMO_JOGAR; 
        }
    }
    return ESTADO_INICIAL;
}

EstadoJogo logica_tela_como_jogar(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || ev->type == ALLEGRO_EVENT_KEY_UP) {
        return ESTADO_INICIAL; 
    }
    return ESTADO_COMO_JOGAR;
}

// Desenho
void desenha_tela_como_jogar(void) {
    if (fundo_como_jogar) {
        al_draw_bitmap(fundo_como_jogar, 0, 0, 0); 
    }
    else {
        al_clear_to_color(al_map_rgb(20, 20, 50));
        al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE, "IMAGEM DE INSTRUÇÕES NAO CARREGADA.");
    }
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA - 50, ALLEGRO_ALIGN_CENTRE, "Pressione qualquer tecla ou clique para voltar ao menu principal.");
}

EstadoJogo logica_transicao_por_toque(EstadoJogo estado_atual, ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (estado_atual == ESTADO_CONTEXTUALIZACAO) return ESTADO_ALERTA;
        else if (estado_atual == ESTADO_ALERTA) return ESTADO_INTERROGATORIO;
    }
    return estado_atual;
}

EstadoJogo logica_tela_interrogatorio(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        int mx = ev->mouse.x;
        int my = ev->mouse.y;

        // Botão Brasil
        if (mx >= BTN_BRASIL_X && mx <= BTN_BRASIL_X + BTN_BRASIL_LARGURA &&
            my >= BTN_BRASIL_Y && my <= BTN_BRASIL_Y + BTN_BRASIL_ALTURA)
        {
            setup_personagem(&jogador);
            return ESTADO_INTERROGATORIO_2;
        }

        // Botão Prisão 
        else if (mx >= BTN_PRISAO_1_X && mx <= BTN_PRISAO_1_X + BTN_PRISAO_1_LARGURA &&
            my >= BTN_PRISAO_1_Y && my <= BTN_PRISAO_1_Y + BTN_PRISAO_1_ALTURA)
        {
            return ESTADO_MAPA_PRISAO;
        }
        // Botão Prisão 
        else if (mx >= BTN_PRISAO_2_X && mx <= BTN_PRISAO_2_X + BTN_PRISAO_2_LARGURA &&
            my >= BTN_PRISAO_2_Y && my <= BTN_PRISAO_2_Y + BTN_PRISAO_2_ALTURA)
        {
            return ESTADO_MAPA_PRISAO;
        }
        // Botão Prisão 
        else if (mx >= BTN_PRISAO_3_X && mx <= BTN_PRISAO_3_X + BTN_PRISAO_3_LARGURA &&
            my >= BTN_PRISAO_3_Y && my <= BTN_PRISAO_3_Y + BTN_PRISAO_3_ALTURA)
        {
            return ESTADO_MAPA_PRISAO;
        }
    }
    return ESTADO_INTERROGATORIO;
}

// Lógica  interrogatório2
EstadoJogo logica_tela_interrogatorio_2(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        int mx = ev->mouse.x;
        int my = ev->mouse.y;

        // Botão Brasil
        if (mx >= BTN2_CERTO_X && mx <= BTN2_CERTO_X + BTN2_CERTO_LARGURA &&
            my >= BTN2_CERTO_Y && my <= BTN2_CERTO_Y + BTN2_CERTO_ALTURA)
        {
            setup_personagem(&jogador);
            return ESTADO_MAPA_BRASIL;
        }

        // Botões Prisao
        if ((mx >= BTN2_ERRADO_1_X && mx <= BTN2_ERRADO_1_X + BTN2_ERRADO_1_LARGURA &&
            my >= BTN2_ERRADO_1_Y && my <= BTN2_ERRADO_1_Y + BTN2_ERRADO_1_ALTURA) ||
            (mx >= BTN2_ERRADO_2_X && mx <= BTN2_ERRADO_2_X + BTN2_ERRADO_2_LARGURA &&
                my >= BTN2_ERRADO_2_Y && my <= BTN2_ERRADO_2_Y + BTN2_ERRADO_2_ALTURA) ||
            (mx >= BTN2_ERRADO_3_X && mx <= BTN2_ERRADO_3_X + BTN2_ERRADO_3_LARGURA &&
                my >= BTN2_ERRADO_3_Y && my <= BTN2_ERRADO_3_Y + BTN2_ERRADO_3_ALTURA))
        {
            return ESTADO_MAPA_PRISAO;
        }
    }
    return ESTADO_INTERROGATORIO_2;
}


// lógica prisão
EstadoJogo logica_mapa_prisao(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (ev->mouse.x >= quadrado_x - 20 && ev->mouse.x <= quadrado_x + 20 &&
            ev->mouse.y >= quadrado_y - 20 && ev->mouse.y <= quadrado_y + 20) {
            arrastando_quadrado = true;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_AXES && arrastando_quadrado) {
        quadrado_x = ev->mouse.x;
        quadrado_y = ev->mouse.y;
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (arrastando_quadrado) {
            arrastando_quadrado = false;
            float dx = quadrado_x - bolinha_x;
            float dy = quadrado_y - bolinha_y;
            float distancia_minima = 25.0f + 20.0f;
            if (dx * dx + dy * dy < distancia_minima * distancia_minima) {
                mg1_tempo_restante = 60;
                mg1_pos = 0;
                mg1_entrada[0] = '\0';
                mg1_venceu = false;
                mg1_perdeu = false;
                mg1_tentativa_errada = false;
                quadrado_x = 100;
                quadrado_y = 100;
                return ESTADO_MINIGAME1;
            }
        }
    }
    return ESTADO_MAPA_PRISAO;
}

// personagem
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
        float w = al_get_bitmap_width(sprite_atual);
        float h = al_get_bitmap_height(sprite_atual);
        al_draw_bitmap(sprite_atual, p->x - w / 2, p->y - h / 2, 0);
    }
    else {
        al_draw_filled_circle(p->x, p->y, 16, al_map_rgb(0, 100, 200));
    }
}

void atualiza_personagem(Personagem* p, ALLEGRO_EVENT* ev) {
    float velocidade = 3.0f;
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
        if (p->x < 0) p->x = 0;
        if (p->x > LARGURA) p->x = LARGURA;
        if (p->y < 0) p->y = 0;
        if (p->y > ALTURA) p->y = ALTURA;
    }
}

// -------------------- MiniGame1--------------------
void mg1_reset(void) {
    mg1_tempo_restante = 60000;
    mg1_pos = 0;
    mg1_entrada[0] = '\0';
    mg1_venceu = false;
    mg1_perdeu = false;
    mg1_tentativa_errada = false;
}

EstadoJogo mg1_logica(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (!mg1_venceu && !mg1_perdeu) {
            mg1_tempo_restante--;
            if (mg1_tempo_restante <= 0) mg1_perdeu = true;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_CHAR) {
        int unicode = ev->keyboard.unichar;
        if (mg1_venceu || mg1_perdeu) {
            if (unicode == 13 && mg1_venceu) {
                // advance to MG2
                mg2_reset();
                return ESTADO_MINIGAME2;
            }
            else if (unicode == 13 && mg1_perdeu) {
                mg1_reset(); // restart mg1
                return ESTADO_INICIAL;
            }
            return ESTADO_MINIGAME1;
        }
        if (unicode == 8 && mg1_pos > 0) {
            mg1_pos--; mg1_entrada[mg1_pos] = '\0';
        }
        else if (unicode >= 32 && unicode <= 126 && mg1_pos < (int)sizeof(mg1_entrada) - 1) {
            mg1_entrada[mg1_pos++] = (char)unicode; mg1_entrada[mg1_pos] = '\0';
        }
        else if (unicode == 13) { // ENTER
            int ok = 0;
            if (strlen(mg1_entrada) > 0) {
                int i; ok = 1;
                for (i = 0; mg1_entrada[i] && mg1_mensagem_correta[i]; ++i) {
                    char a = mg1_entrada[i];
                    char b = mg1_mensagem_correta[i];
                    if (a >= 'a' && a <= 'z') a -= 32;
                    if (b >= 'a' && b <= 'z') b -= 32;
                    if (a != b) { ok = 0; break; }
                }
                if (mg1_entrada[i] || mg1_mensagem_correta[i]) ok = 0;
            }
            if (ok) { mg1_venceu = true; }
            else { mg1_tentativa_errada = true; mg1_entrada[0] = '\0'; mg1_pos = 0; }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }
    return ESTADO_MINIGAME1;
}

void mg1_draw(void) {
    float scale_x = LARGURA / 800.0f;
    float scale_y = ALTURA / 600.0f;
    float scale = (scale_x + scale_y) / 2.0f;

    al_clear_to_color(al_map_rgb(10, 10, 30));

    // Título
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255),
        LARGURA / 2, 100 * scale_y, ALLEGRO_ALIGN_CENTRE,
        "MENSAGEM CODIFICADA:");

    // Mensagem
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 0),
        LARGURA / 2, 160 * scale_y, ALLEGRO_ALIGN_CENTRE,
        mg1_mensagem_codificada);

    // Texto
    al_draw_text(fonte_builtin, al_map_rgb(180, 180, 255),
        LARGURA / 2, 320 * scale_y, ALLEGRO_ALIGN_CENTRE,
        "Digite a mensagem decifrada:");

    // Entrada do jogador
    al_draw_text(fonte_builtin, al_map_rgb(0, 255, 0),
        LARGURA / 2, 380 * scale_y, ALLEGRO_ALIGN_CENTRE,
        mg1_entrada);

    // Tempo 
    char buf[64];
    snprintf(buf, sizeof(buf), "Tempo restante: %ds", mg1_tempo_restante);
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255),
        40 * scale_x, 30 * scale_y, 0, buf);

    // Dica
    al_draw_text(fonte_builtin, al_map_rgb(255, 200, 0),
        LARGURA / 2, ALTURA - (120 * scale_y), ALLEGRO_ALIGN_CENTRE,
        mg1_dica);

    // Mensagens de resultado
    if (mg1_venceu) {
        al_draw_text(fonte_builtin, al_map_rgb(0, 255, 0),
            LARGURA / 2, ALTURA / 2 + (40 * scale_y), ALLEGRO_ALIGN_CENTRE,
            "Correto! Pressione ENTER para continuar.");
    }
    if (mg1_perdeu) {
        al_draw_text(fonte_builtin, al_map_rgb(255, 0, 0),
            LARGURA / 2, ALTURA / 2 + (40 * scale_y), ALLEGRO_ALIGN_CENTRE,
            "Tempo esgotado, você perdeu! precione ENTER para reiniciar o jogo.");
    }
    if (mg1_tentativa_errada) {
        al_draw_text(fonte_builtin, al_map_rgb(255, 50, 50),
            LARGURA / 2, ALTURA / 2 + (80 * scale_y), ALLEGRO_ALIGN_CENTRE,
            "Resposta incorreta. Tente novamente.");
    }
}

// -------------------- MiniGame2 --------------------
void mg2_reset(void) {
    srand((unsigned)time(NULL));
    mg2_jogX = MG2_TILE * 1.5f;
    mg2_jogY = MG2_TILE * 1.5f;
    mg2_tempo = MG2_TEMPO_MAX;
    mg2_fim = false;
    mg2_venceu = false;
    mg2_podeTentar = false;
    for (int i = 0; i < MG2_NUM_GUARDAS; i++) {
        mg2_guardas[i].x = MG2_TILE * (rand() % 18 + 1);
        mg2_guardas[i].y = MG2_TILE * (rand() % 13 + 1);
        mg2_guardas[i].velX = ((rand() % 2) * 2 - 1) * 2;
        mg2_guardas[i].velY = ((rand() % 2) * 2 - 1) * 2;
        mg2_guardas[i].tempoTroca = rand() % 120 + 60;
    }
    for (int k = 0; k < 4; k++) mg2_teclas[k] = false;
}

EstadoJogo mg2_logica(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_TIMER && !mg2_fim) {
        float novoX = mg2_jogX, novoY = mg2_jogY;
        if (mg2_teclas[0]) novoY -= MG2_VELOC;
        if (mg2_teclas[1]) novoX -= MG2_VELOC;
        if (mg2_teclas[2]) novoY += MG2_VELOC;
        if (mg2_teclas[3]) novoX += MG2_VELOC;
        int linha, coluna;
        linha = (int)(novoY / MG2_TILE); coluna = (int)(mg2_jogX / MG2_TILE);
        if (!mg2_colisao(linha, coluna)) mg2_jogY = novoY;
        linha = (int)(mg2_jogY / MG2_TILE); coluna = (int)(novoX / MG2_TILE);
        if (!mg2_colisao(linha, coluna)) mg2_jogX = novoX;

        for (int i = 0; i < MG2_NUM_GUARDAS; i++) {
            mg2_guardas[i].tempoTroca--;
            if (mg2_guardas[i].tempoTroca <= 0) {
                mg2_guardas[i].velX = ((rand() % 2) * 2 - 1) * 2;
                mg2_guardas[i].velY = ((rand() % 2) * 2 - 1) * 2;
                mg2_guardas[i].tempoTroca = rand() % 120 + 60;
            }
            float novoGX = mg2_guardas[i].x + mg2_guardas[i].velX;
            float novoGY = mg2_guardas[i].y + mg2_guardas[i].velY;
            int lG = (int)(mg2_guardas[i].y / MG2_TILE);
            int cG = (int)(novoGX / MG2_TILE);
            if (!mg2_colisao(lG, cG)) mg2_guardas[i].x = novoGX;
            else mg2_guardas[i].velX *= -1;
            lG = (int)(novoGY / MG2_TILE);
            cG = (int)(mg2_guardas[i].x / MG2_TILE);
            if (!mg2_colisao(lG, cG)) mg2_guardas[i].y = novoGY;
            else mg2_guardas[i].velY *= -1;
            float dx = mg2_guardas[i].x - mg2_jogX;
            float dy = mg2_guardas[i].y - mg2_jogY;
            if (dx * dx + dy * dy < (MG2_TILE / 2.0f) * (MG2_TILE / 2.0f)) {
                mg2_tempo -= 3.0f;
                mg2_jogX = MG2_TILE * 1.5f;
                mg2_jogY = MG2_TILE * 1.5f;
            }
        }
        mg2_tempo -= 1.0f / 60.0f;
        if (mg2_tempo <= 0) {
            mg2_fim = true; mg2_venceu = false; mg2_podeTentar = true;
        }
        if (mg2_jogX > (18 * MG2_TILE) && mg2_jogY > (13 * MG2_TILE)) {
            mg2_fim = true; mg2_venceu = true;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (ev->keyboard.keycode) {
        case ALLEGRO_KEY_W: mg2_teclas[0] = true; break;
        case ALLEGRO_KEY_A: mg2_teclas[1] = true; break;
        case ALLEGRO_KEY_S: mg2_teclas[2] = true; break;
        case ALLEGRO_KEY_D: mg2_teclas[3] = true; break;
        case ALLEGRO_KEY_ENTER:
            if (mg2_podeTentar) {
                mg2_reset();
                return ESTADO_INICIAL;
            }
            break;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_UP) {
        switch (ev->keyboard.keycode) {
        case ALLEGRO_KEY_W: mg2_teclas[0] = false; break;
        case ALLEGRO_KEY_A: mg2_teclas[1] = false; break;
        case ALLEGRO_KEY_S: mg2_teclas[2] = false; break;
        case ALLEGRO_KEY_D: mg2_teclas[3] = false; break;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }

    if (mg2_fim && mg2_venceu) {
        setup_personagem(&jogador);
        return ESTADO_MAPA_EXILIO;
    }
    return ESTADO_MINIGAME2;
}

void mg2_draw() {
    // Limpa a tela
    al_clear_to_color(al_map_rgb(15, 15, 30));


    const int COLS = 20;
    const int ROWS = 15;
    float tile = (float)LARGURA / COLS;
    float total_height = ROWS * tile;
    float offset_x = 0; 
    float offset_y = (ALTURA - total_height) / 2.0f; 
    float scale_x = tile / MG2_TILE;
    float scale_y = tile / MG2_TILE;
    float scale = (scale_x + scale_y) / 2.0f;



    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (mg2_labirinto[i][j] == 1) {
                al_draw_filled_rectangle(
                    offset_x + j * tile,
                    offset_y + i * tile,
                    offset_x + (j + 1) * tile,
                    offset_y + (i + 1) * tile,
                    al_map_rgb(40, 40, 40)
                );
            }
        }
    }

    // Saída
    al_draw_filled_rectangle(
        offset_x + 18 * tile,
        offset_y + 13 * tile,
        offset_x + 19 * tile,
        offset_y + 14 * tile,
        al_map_rgb(0, 255, 0)
    );

    // Jogador
    al_draw_filled_rectangle(
        offset_x + mg2_jogX * scale_x - 10 * scale,
        offset_y + mg2_jogY * scale_y - 10 * scale,
        offset_x + mg2_jogX * scale_x + 10 * scale,
        offset_y + mg2_jogY * scale_y + 10 * scale,
        al_map_rgb(0, 150, 255)
    );
    // Guardas
    for (int i = 0; i < MG2_NUM_GUARDAS; i++) {
        float center_x = offset_x + mg2_guardas[i].x * scale_x;
        float center_y = offset_y + mg2_guardas[i].y * scale_y;
        float size = 20 * scale;
        float half_size = 10 * scale;
        al_draw_scaled_bitmap(
            mg2_guarda_img,
            0, 0, al_get_bitmap_width(mg2_guarda_img), al_get_bitmap_height(mg2_guarda_img),
            center_x - half_size, 
            center_y - half_size, 
            size, size,           
            0
        );
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "Tempo: %.1f", mg2_tempo);

    // Cronômetro no topo fixo da tela
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255),
        30, 20, 0, buf);

    // Dica logo abaixo
    al_draw_text(fonte_builtin, al_map_rgb(255, 230, 0),
        30, 60, 0,
        "Dica: Evite os guardas e chegue à saída!");

    if (mg2_fim) {
        if (mg2_venceu) {
            al_draw_text(fonte_builtin, al_map_rgb(0, 255, 0),
                LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE,
                "Você escapou! Voltando ao mapa...");
        }
        else {
            al_draw_text(fonte_builtin, al_map_rgb(255, 0, 0),
                LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE,
                "Você foi pego e perdeu! Pressione ENTER para reiniciar o jogo.");
        }
    }
}




// -------------------- MiniGame3 --------------------
void mg3_reset(void) {
    float larguraPeca = 80.0f;
    float espacamentoX = 140.0f;
    float inicioX = (LARGURA - (4 * larguraPeca + 3 * espacamentoX)) / 2.0f;
    float linha1Y = ALTURA / 2.0f - 150.0f;
    float linha2Y = ALTURA / 2.0f + 50.0f;
    float espacamentoInferior = 40.0f;
    float totalLargura = 6 * larguraPeca + 5 * espacamentoInferior;
    float inicioInferiorX = (LARGURA - totalLargura) / 2.0f;
    float posYInferior = ALTURA - 160.0f;

    for (int i = 0; i < 6; ++i) {
        mg3_pecas[i].x = inicioInferiorX + i * (larguraPeca + espacamentoInferior);
        mg3_pecas[i].y = posYInferior;
        mg3_pecas[i].alvoX = inicioX + (i % 4) * (larguraPeca + espacamentoX);
        mg3_pecas[i].alvoY = (i < 4) ? linha1Y : linha2Y;
        mg3_pecas[i].correto = false;
        mg3_pecas[i].arrastando = false;
        mg3_pecas[i].offsetX = mg3_pecas[i].offsetY = 0.0f;
        mg3_pecas[i].tipo = i + 1;
    }
    mg3_tempo = MG3_TEMPO_LIMITE;
    mg3_ganhou = mg3_perdeu = false;
}

static float distf(float ax, float ay, float bx, float by) {
    float dx = ax - bx; float dy = ay - by; return sqrtf(dx * dx + dy * dy);
}

EstadoJogo mg3_logica(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (!mg3_ganhou && !mg3_perdeu) {
            mg3_tempo -= 1.0f / 60.0f;
            if (mg3_tempo <= 0.0f) { mg3_perdeu = true; }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && !mg3_ganhou && !mg3_perdeu) {
        for (int i = 0; i < 6; ++i) {
            if (!mg3_pecas[i].correto) {
                const float HALF_W = 67.0f;
                const float HALF_H = 63.5f;

                if (ev->mouse.x >= mg3_pecas[i].x - HALF_W && ev->mouse.x <= mg3_pecas[i].x + HALF_W &&
                    ev->mouse.y >= mg3_pecas[i].y - HALF_H && ev->mouse.y <= mg3_pecas[i].y + HALF_H)
                {
                    mg3_pecas[i].arrastando = true;
                    mg3_pecas[i].offsetX = ev->mouse.x - mg3_pecas[i].x;
                    mg3_pecas[i].offsetY = ev->mouse.y - mg3_pecas[i].y;
                }
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
        for (int i = 0; i < 6; ++i) {
            if (mg3_pecas[i].arrastando) {
                mg3_pecas[i].x = ev->mouse.x - mg3_pecas[i].offsetX;
                mg3_pecas[i].y = ev->mouse.y - mg3_pecas[i].offsetY;
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        for (int i = 0; i < 6; ++i) {
            if (mg3_pecas[i].arrastando) {
                mg3_pecas[i].arrastando = false;
                float dx = mg3_pecas[i].x - mg3_pecas[i].alvoX;
                float dy = mg3_pecas[i].y - mg3_pecas[i].alvoY;
                const float TOLERANCIA_MAXIMA_SQ = 1600.0f; 
                if ((dx * dx + dy * dy) < TOLERANCIA_MAXIMA_SQ) {
                    mg3_pecas[i].x = mg3_pecas[i].alvoX;
                    mg3_pecas[i].y = mg3_pecas[i].alvoY;
                    mg3_pecas[i].correto = true;
                }
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER && (mg3_ganhou || mg3_perdeu)) {
            if (mg3_ganhou) {
                mg3_completed = true;
                mg4_ativado = true;
                setup_personagem(&jogador);
                return ESTADO_MAPA_EXILIO;
            }
            else {
                mg3_reset();
                return ESTADO_MAPA_PRISAO;
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }

    if (!mg3_ganhou && !mg3_perdeu) {
        bool todos = true;
        for (int i = 0; i < 6; ++i) if (!mg3_pecas[i].correto) { todos = false; break; }
        if (todos) { mg3_ganhou = true; }
    }

    return ESTADO_MINIGAME3;
}

void mg3_draw(void) {
    al_clear_to_color(al_map_rgb(15, 15, 30));
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, 60, ALLEGRO_ALIGN_CENTRE, "Monte o transmissor: arraste as peças para os locais corretos!");
    char tempoTxt[64];
    snprintf(tempoTxt, sizeof(tempoTxt), "Tempo restante: %.0f", mg3_tempo);
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 0), 50, 50, 0, tempoTxt);

    for (int i = 0; i < 6; i++) {
        ALLEGRO_BITMAP* sprite_peca = NULL;
        ALLEGRO_COLOR cor_fallback = al_map_rgb(200, 200, 200);
        switch (mg3_pecas[i].tipo) {
        case 1:
            sprite_peca = mg3_peca_1_img;
            break;
        case 2:
            sprite_peca = mg3_peca_2_img;
            break;
        case 3:
            sprite_peca = mg3_peca_3_img;
            break;
        case 4:
            sprite_peca = mg3_peca_4_img;
            break;
        case 5:
            sprite_peca = mg3_peca_5_img;
            break;
        case 6:
            sprite_peca = mg3_peca_6_img;
            break;
        }

        float peca_largura = 134.0f;
        float peca_altura = 96.0f;

        if (sprite_peca) {
            al_draw_scaled_bitmap(
                sprite_peca,
                0, 0, al_get_bitmap_width(sprite_peca), al_get_bitmap_height(sprite_peca),
                mg3_pecas[i].x - peca_largura / 2, 
                mg3_pecas[i].y - peca_altura / 2,  
                peca_largura, peca_altura,
                0
            );
        }

    }
    for (int i = 0; i < 6; i++) {
        float target_x = mg3_pecas[i].alvoX;
        float target_y = mg3_pecas[i].alvoY;
        float target_size = 100.0f; 

        al_draw_filled_rectangle(
            target_x - target_size / 2,
            target_y - target_size / 2,
            target_x + target_size / 2,
            target_y + target_size / 2,
            al_map_rgba(200, 200, 200, 50) 
        );

        if (mg3_pecas[i].correto) {
            al_draw_rectangle(
                target_x - target_size / 2,
                target_y - target_size / 2,
                target_x + target_size / 2,
                target_y + target_size / 2,
                al_map_rgb(0, 255, 0), 
                5.0f 
            );
        }
    }
    if (mg3_ganhou) {
        al_draw_text(fonte_builtin, al_map_rgb(0, 255, 0), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE, "Rádio montado com sucesso! Pressione ENTER para voltar.");
    }
    else if (mg3_perdeu) {
        al_draw_text(fonte_builtin, al_map_rgb(255, 0, 0), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE, "Transmissão falhou! Você foi pego tentando roubar as peças, pressione ENTER vá para a prisão.");
    }
}

// -------------------- MiniGame4 --------------------
void mg4_reset(void) {
    mg4_rodada = 0;
    mg4_alcance = 0; mg4_risco = 0;
    mg4_mostrandoFeedback = false; mg4_feedbackTimer = 0.0;
    mg4_finalizado = false; mg4_venceu = false;
    srand((unsigned)time(NULL));
    for (int r = 0; r < MG4_RODADAS; ++r) {
        int a, b, c;
        do { a = rand() % mg4_poolSize; b = rand() % mg4_poolSize; c = rand() % mg4_poolSize; } while (a == b || a == c || b == c);
        mg4_opcoes[r][0] = a; mg4_opcoes[r][1] = b; mg4_opcoes[r][2] = c;
    }
}

EstadoJogo mg4_logica(ALLEGRO_EVENT* ev) {
    const double FEEDBACK_DUR = 2.0;
    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (mg4_mostrandoFeedback) {
            mg4_feedbackTimer += 1.0 / 60.0;
            if (mg4_feedbackTimer >= FEEDBACK_DUR) {
                mg4_mostrandoFeedback = false;
                mg4_feedbackTimer = 0.0;
                mg4_rodada++;
                if (mg4_alcance >= 100) { mg4_finalizado = true; mg4_venceu = true; }
                else if (mg4_risco >= 100) { mg4_finalizado = true; mg4_venceu = false; }
                else if (mg4_rodada >= MG4_RODADAS) {
                    mg4_finalizado = true;
                    mg4_venceu = (mg4_alcance >= 100 && mg4_risco < 100);
                }
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (!mg4_finalizado && !mg4_mostrandoFeedback) {
            int escolha = -1;
            if (ev->keyboard.keycode == ALLEGRO_KEY_1) escolha = 0;
            else if (ev->keyboard.keycode == ALLEGRO_KEY_2) escolha = 1;
            else if (ev->keyboard.keycode == ALLEGRO_KEY_3) escolha = 2;
            if (escolha != -1) {
                int idx = mg4_opcoes[mg4_rodada][escolha];
                Pais* p = &mg4_pool[idx];
                mg4_alcance += p->alcance;
                mg4_risco += p->risco;
                if (mg4_alcance < 0) mg4_alcance = 0;
                if (mg4_risco < 0) mg4_risco = 0;
                if (mg4_alcance > 200) mg4_alcance = 200;
                if (mg4_risco > 200) mg4_risco = 200;
                snprintf(mg4_feedbackTexto, sizeof(mg4_feedbackTexto), "%s: %s  Alcance +%d  Risco %+d", p->nome, p->descricao, p->alcance, p->risco);
                mg4_mostrandoFeedback = true;
                mg4_feedbackTimer = 0.0;
            }
        }
        else if (mg4_finalizado && !mg4_venceu) {
            if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                mg4_reset();
                return ESTADO_MAPA_PRISAO;
            }
        }
        else if (mg4_finalizado && mg4_venceu) {
            if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                setup_personagem(&jogador);
                return ESTADO_TELA_VOCE_GANHOU;
            }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }

    return ESTADO_MINIGAME4;
}

void mg4_draw(void) {
    float scale_x = (float)LARGURA / 900.0f;
    float scale_y = (float)ALTURA / 600.0f;
    ALLEGRO_FONT* fonte_grande = al_create_builtin_font();

    al_clear_to_color(al_map_rgb(12, 12, 20));

    float centroX = LARGURA / 2.0f;

    al_draw_textf(fonte_grande, al_map_rgb(255, 255, 255),
        centroX, 30 * scale_y, ALLEGRO_ALIGN_CENTER,
        "A Voz da Resistência - Rodada %d / %d", mg4_rodada + 1, MG4_RODADAS);

    al_draw_text(fonte_grande, al_map_rgb(220, 220, 180),
        centroX, 70 * scale_y, ALLEGRO_ALIGN_CENTER,
        "Do exílio, espalhe a verdade. Escolha para onde enviar os jornais (1/2/3).");

    al_draw_textf(fonte_grande, al_map_rgb(200, 255, 200),
        centroX, 110 * scale_y, ALLEGRO_ALIGN_CENTER,
        "Alcance: %d%%", (mg4_alcance < 0 ? 0 : mg4_alcance > 100 ? 100 : mg4_alcance));

    float barra_w = 600 * scale_x;
    float barra_h = 25 * scale_y;
    float barra_x = centroX - barra_w / 2;
    float barra_y = 150 * scale_y;

    al_draw_filled_rectangle(barra_x, barra_y,
        barra_x + barra_w, barra_y + barra_h,
        al_map_rgb(60, 60, 60));

    al_draw_filled_rectangle(barra_x, barra_y,
        barra_x + ((mg4_alcance < 0 ? 0 : mg4_alcance > 100 ? 100 : mg4_alcance) * (barra_w / 100.0f)),
        barra_y + barra_h,
        al_map_rgb(30, 200, 30));

    al_draw_textf(fonte_grande, al_map_rgb(255, 200, 200),
        centroX, 200 * scale_y, ALLEGRO_ALIGN_CENTER,
        "Risco: %d%%", (mg4_risco < 0 ? 0 : mg4_risco > 100 ? 100 : mg4_risco));

    barra_y = 230 * scale_y;

    al_draw_filled_rectangle(barra_x, barra_y,
        barra_x + barra_w, barra_y + barra_h,
        al_map_rgb(60, 60, 60));

    al_draw_filled_rectangle(barra_x, barra_y,
        barra_x + ((mg4_risco < 0 ? 0 : mg4_risco > 100 ? 100 : mg4_risco) * (barra_w / 100.0f)),
        barra_y + barra_h,
        al_map_rgb(200, 30, 30));

    if (mg4_mostrandoFeedback) {
        al_draw_textf(fonte_grande, al_map_rgb(255, 255, 180),
            centroX, 350 * scale_y, ALLEGRO_ALIGN_CENTER, "%s", mg4_feedbackTexto);
        al_draw_text(fonte_grande, al_map_rgb(200, 200, 200),
            centroX, 390 * scale_y, ALLEGRO_ALIGN_CENTER, "Aguardando...");
    }
    else if (!mg4_finalizado) {
        int baseY = 280 * scale_y;   
        int espacamento = 100 * scale_y;

        int i0 = mg4_opcoes[mg4_rodada][0], i1 = mg4_opcoes[mg4_rodada][1], i2 = mg4_opcoes[mg4_rodada][2];

        al_draw_textf(fonte_grande, al_map_rgb(220, 220, 255),
            centroX, baseY, ALLEGRO_ALIGN_CENTER, "[1] %s", mg4_pool[i0].nome);
        al_draw_text(fonte_grande, al_map_rgb(200, 200, 200),
            centroX, baseY + 30 * scale_y, ALLEGRO_ALIGN_CENTER, mg4_pool[i0].descricao);

        al_draw_textf(fonte_grande, al_map_rgb(220, 220, 255),
            centroX, baseY + espacamento, ALLEGRO_ALIGN_CENTER, "[2] %s", mg4_pool[i1].nome);
        al_draw_text(fonte_grande, al_map_rgb(200, 200, 200),
            centroX, baseY + espacamento + 30 * scale_y, ALLEGRO_ALIGN_CENTER, mg4_pool[i1].descricao);

        al_draw_textf(fonte_grande, al_map_rgb(220, 220, 255),
            centroX, baseY + 2 * espacamento, ALLEGRO_ALIGN_CENTER, "[3] %s", mg4_pool[i2].nome);
        al_draw_text(fonte_grande, al_map_rgb(200, 200, 200),
            centroX, baseY + 2 * espacamento + 30 * scale_y, ALLEGRO_ALIGN_CENTER, mg4_pool[i2].descricao);

        al_draw_text(fonte_grande, al_map_rgb(240, 240, 160),
            centroX, ALTURA - 80 * scale_y, ALLEGRO_ALIGN_CENTER,
            "Escolha 1, 2 ou 3 para enviar o jornal.");
    }

    if (mg4_finalizado) {
        if (mg4_venceu) {
            al_draw_textf(fonte_grande, al_map_rgb(0, 255, 0),
                centroX, ALTURA / 2 - 30 * scale_y, ALLEGRO_ALIGN_CENTER,
                "VITÓRIA: Sua mensagem alcançou o mundo!");
            al_draw_textf(fonte_grande, al_map_rgb(220, 220, 220),
                centroX, ALTURA / 2 + 20 * scale_y, ALLEGRO_ALIGN_CENTER,
                "Pressione ENTER para voltar ao mapa.");
        }
        else {
            al_draw_textf(fonte_grande, al_map_rgb(255, 80, 80),
                centroX, ALTURA / 2 - 30 * scale_y, ALLEGRO_ALIGN_CENTER,
                "DERROTA: Você foi rastreado!");
            al_draw_textf(fonte_grande, al_map_rgb(220, 220, 220),
                centroX, ALTURA / 2 + 20 * scale_y, ALLEGRO_ALIGN_CENTER,
                "Pressione ENTER e volte para a prisão!");
        }
    }


}

// -------------------- MG5--------------------

static int mg5_clampi(int v, int a, int b) { if (v < a) return a; if (v > b) return b; return v; }

bool mg5_cell_free(int gx, int gy) {
    if (gx < 0 || gy < 0 || gx >= MG5_COLS || gy >= MG5_ROWS) return false;
    return mg5_map[gy][gx] == 0;
}
void mg5_pick_random_free_cell(int* out_gx, int* out_gy) {
    int gx, gy;
    do {
        gx = rand() % MG5_COLS;
        gy = rand() % MG5_ROWS;
    } while (!mg5_cell_free(gx, gy));
    *out_gx = gx; *out_gy = gy;
}
int mg5_neighbors[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

void mg5_guard_choose_new_target(MG5_Guard* g) {
    int choices[5][2];
    int ccount = 0;
    choices[ccount][0] = g->gx; choices[ccount][1] = g->gy; ccount++;
    for (int i = 0; i < 4; i++) {
        int nx = g->gx + mg5_neighbors[i][0];
        int ny = g->gy + mg5_neighbors[i][1];
        if (mg5_cell_free(nx, ny)) {
            choices[ccount][0] = nx;
            choices[ccount][1] = ny;
            ccount++;
        }
    }
    int pick = rand() % ccount;
    g->target_gx = choices[pick][0];
    g->target_gy = choices[pick][1];
    g->moving = !(g->target_gx == g->gx && g->target_gy == g->gy);
}

void mg5_reset(void) {
    srand((unsigned)time(NULL));
    mg5_scale_x = (float)LARGURA / MG5_BASE_W;
    mg5_scale_y = (float)ALTURA / MG5_BASE_H;
    mg5_initialized = true;
    mg5_player.gx = 1; mg5_player.gy = 1;
    mg5_player.x = mg5_player.gx * MG5_TILE + MG5_TILE / 2;
    mg5_player.y = mg5_player.gy * MG5_TILE + MG5_TILE / 2;
    mg5_player_start = mg5_player;

    mg5_exit_gx = MG5_COLS - 2; mg5_exit_gy = MG5_ROWS - 2;

    // Cartinhas
    int placed = 0;
    while (placed < MG5_NUM_ITEMS) {
        int gx, gy; mg5_pick_random_free_cell(&gx, &gy);
        if (gx == mg5_player.gx && gy == mg5_player.gy) continue;
        if (gx == mg5_exit_gx && gy == mg5_exit_gy) continue;
        bool dup = false;
        for (int i = 0; i < placed; i++) if (mg5_items[i].gx == gx && mg5_items[i].gy == gy) { dup = true; break; }
        if (dup) continue;
        mg5_items[placed].gx = gx; mg5_items[placed].gy = gy; mg5_items[placed].taken = false;
        placed++;
    }

    // Guardas
    for (int i = 0; i < MG5_NUM_GUARDS; i++) {
        int gx, gy;
        do { mg5_pick_random_free_cell(&gx, &gy); } while (gx == mg5_player.gx && gy == mg5_player.gy);
        mg5_guards[i].gx = gx; mg5_guards[i].gy = gy;
        mg5_guards[i].x = gx * MG5_TILE + MG5_TILE / 2;
        mg5_guards[i].y = gy * MG5_TILE + MG5_TILE / 2;
        mg5_guards[i].target_gx = gx; mg5_guards[i].target_gy = gy;
        mg5_guards[i].moving = false;
    }
    for (int i = 0; i < MG5_NUM_GUARDS; ++i) mg5_guard_choose_new_target(&mg5_guards[i]);

    mg5_remaining = MG5_START_TIME;
    mg5_game_over = false; mg5_win = false; mg5_can_retry = false;
}

EstadoJogo mg5_logica(ALLEGRO_EVENT* ev) {
    static bool key_w = false, key_a = false, key_s = false, key_d = false;
    const float PLAYER_SPEED = 3.0f;
    const float GUARD_SPEED = 1.6f;
    if (!mg5_initialized) mg5_reset();

    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (!mg5_game_over) {
            float nx = mg5_player.x, ny = mg5_player.y;
            if (key_w) ny -= PLAYER_SPEED;
            if (key_s) ny += PLAYER_SPEED;
            if (key_a) nx -= PLAYER_SPEED;
            if (key_d) nx += PLAYER_SPEED;
            bool collideX = false, collideY = false;
            float cornersX[4][2] = { {nx - 10, mg5_player.y - 10},{nx + 10, mg5_player.y - 10},{nx - 10, mg5_player.y + 10},{nx + 10, mg5_player.y + 10} };
            for (int c = 0; c < 4; c++) { int cgx = (int)(cornersX[c][0] / MG5_TILE); int cgy = (int)(cornersX[c][1] / MG5_TILE); if (cgx < 0 || cgy < 0 || cgx >= MG5_COLS || cgy >= MG5_ROWS || mg5_map[cgy][cgx] == 1) { collideX = true; break; } }
            float cornersY[4][2] = { {mg5_player.x - 10, ny - 10},{mg5_player.x + 10, ny - 10},{mg5_player.x - 10, ny + 10},{mg5_player.x + 10, ny + 10} };
            for (int c = 0; c < 4; c++) { int cgx = (int)(cornersY[c][0] / MG5_TILE); int cgy = (int)(cornersY[c][1] / MG5_TILE); if (cgx < 0 || cgy < 0 || cgx >= MG5_COLS || cgy >= MG5_ROWS || mg5_map[cgy][cgx] == 1) { collideY = true; break; } }
            if (!collideX) mg5_player.x = nx;
            if (!collideY) mg5_player.y = ny;
            mg5_player.gx = (int)(mg5_player.x / MG5_TILE);
            mg5_player.gy = (int)(mg5_player.y / MG5_TILE);

            for (int g = 0; g < MG5_NUM_GUARDS; ++g) {
                MG5_Guard* G = &mg5_guards[g];
                if (!G->moving && (G->target_gx != G->gx || G->target_gy != G->gy)) G->moving = true;
                if (G->moving) {
                    float tx = G->target_gx * MG5_TILE + MG5_TILE / 2;
                    float ty = G->target_gy * MG5_TILE + MG5_TILE / 2;
                    float dx = tx - G->x;
                    float dy = ty - G->y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= GUARD_SPEED) {
                        G->x = tx; G->y = ty; G->gx = G->target_gx; G->gy = G->target_gy; G->moving = false;
                        mg5_guard_choose_new_target(G);
                    }
                    else { G->x += GUARD_SPEED * (dx / dist); G->y += GUARD_SPEED * (dy / dist); }
                }
                else {
                    if (rand() % 30 == 0) mg5_guard_choose_new_target(G);
                }
            }

            bool seen = false;
            for (int g = 0; g < MG5_NUM_GUARDS; ++g) if (mg5_guards[g].gx == mg5_player.gx && mg5_guards[g].gy == mg5_player.gy) { seen = true; break; }
            if (seen) {
                mg5_remaining -= MG5_PENALTY;
                mg5_player = mg5_player_start;
                mg5_player.gx = (int)(mg5_player.x / MG5_TILE);
                mg5_player.gy = (int)(mg5_player.y / MG5_TILE);
                if (mg5_remaining <= 0.0f) { mg5_game_over = true; mg5_win = false; mg5_can_retry = true; }
            }

            for (int i = 0; i < MG5_NUM_ITEMS; i++) {
                if (!mg5_items[i].taken && mg5_items[i].gx == mg5_player.gx && mg5_items[i].gy == mg5_player.gy) mg5_items[i].taken = true;
            }
            bool all = true;
            for (int i = 0; i < MG5_NUM_ITEMS; i++) if (!mg5_items[i].taken) { all = false; break; }
            if (all && mg5_player.gx == mg5_exit_gx && mg5_player.gy == mg5_exit_gy) { mg5_game_over = true; mg5_win = true; }

            mg5_remaining -= 1.0f / 60.0f;
            if (mg5_remaining <= 0.0f && !mg5_game_over) { mg5_game_over = true; mg5_win = false; mg5_can_retry = true; }
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (ev->keyboard.keycode == ALLEGRO_KEY_W) key_w = true;
        if (ev->keyboard.keycode == ALLEGRO_KEY_A) key_a = true;
        if (ev->keyboard.keycode == ALLEGRO_KEY_S) key_s = true;
        if (ev->keyboard.keycode == ALLEGRO_KEY_D) key_d = true;

        if (mg5_game_over && !mg5_win && ev->keyboard.keycode == ALLEGRO_KEY_ENTER && mg5_can_retry) {
            
            mg5_reset();
            return ESTADO_MAPA_PRISAO;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_KEY_UP) {
        if (ev->keyboard.keycode == ALLEGRO_KEY_W) key_w = false;
        if (ev->keyboard.keycode == ALLEGRO_KEY_A) key_a = false;
        if (ev->keyboard.keycode == ALLEGRO_KEY_S) key_s = false;
        if (ev->keyboard.keycode == ALLEGRO_KEY_D) key_d = false;
    }
    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }

    if (mg5_game_over && mg5_win) {
        mg6_reset();
        return ESTADO_MINIGAME6;
    }

    return ESTADO_MINIGAME5;
}

void mg5_draw(void) {
    if (!mg5_initialized) mg5_reset();

    float sx = mg5_scale_x, sy = mg5_scale_y;

    al_clear_to_color(al_map_rgb(14, 14, 20));

    for (int r = 0; r < MG5_ROWS; r++) {
        for (int c = 0; c < MG5_COLS; c++) {
            float x0 = c * MG5_TILE * sx;
            float y0 = r * MG5_TILE * sy;
            float x1 = (c + 1) * MG5_TILE * sx;
            float y1 = (r + 1) * MG5_TILE * sy;
            if (mg5_map[r][c] == 1)
                al_draw_filled_rectangle(x0, y0, x1, y1, al_map_rgb(60, 60, 60));
            else
                al_draw_filled_rectangle(x0, y0, x1, y1, al_map_rgb(22, 22, 30));
        }
    }

    al_draw_filled_rectangle(mg5_exit_gx * MG5_TILE * sx + 6 * sx, mg5_exit_gy * MG5_TILE * sy + 6 * sy, (mg5_exit_gx * MG5_TILE + MG5_TILE) * sx - 6 * sx, (mg5_exit_gy * MG5_TILE + MG5_TILE) * sy - 6 * sy, al_map_rgb(0, 200, 0));

    int left = 0;
    const float ITEM_RAIO = (MG5_TILE / 4);
    const float ITEM_DIAMETRO = ITEM_RAIO * 2.0f; 
    const float ITEM_ESCALA = (sx + sy) / 2.0f;

    for (int i = 0; i < MG5_NUM_ITEMS; i++) {
        if (!mg5_items[i].taken) {
            float ix = (mg5_items[i].gx * MG5_TILE + MG5_TILE / 2) * sx;
            float iy = (mg5_items[i].gy * MG5_TILE + MG5_TILE / 2) * sy;

            if (mg5_coletavel_img) { 
                al_draw_scaled_bitmap(
                    mg5_coletavel_img,
                    0, 0, al_get_bitmap_width(mg5_coletavel_img), al_get_bitmap_height(mg5_coletavel_img),
                    ix - (ITEM_RAIO * ITEM_ESCALA), // Posição X 
                    iy - (ITEM_RAIO * ITEM_ESCALA), // Posição Y
                    ITEM_DIAMETRO * ITEM_ESCALA,    // Largura 
                    ITEM_DIAMETRO * ITEM_ESCALA,    // Altura 
                    0
                );
            }
            else {
                al_draw_filled_circle(ix, iy, ITEM_RAIO * ITEM_ESCALA, al_map_rgb(230, 200, 40));
            }

            left++;
        }
    }

    // Guardas
    const float GUARDA_RAIO = (MG5_TILE / 3 - 2); 
    const float GUARDA_DIAMETRO = GUARDA_RAIO * 2.0f; 
    const float GUARDA_ESCALA = (sx + sy) / 2.0f; 

    for (int g = 0; g < MG5_NUM_GUARDS; ++g) {
        float gx = mg5_guards[g].x * sx;
        float gy = mg5_guards[g].y * sy;

        
        if (mg5_perseguidor_img) { 
            al_draw_scaled_bitmap(
                mg5_perseguidor_img,
                0, 0, al_get_bitmap_width(mg5_perseguidor_img), al_get_bitmap_height(mg5_perseguidor_img),
                gx - (GUARDA_RAIO * GUARDA_ESCALA), 
                gy - (GUARDA_RAIO * GUARDA_ESCALA), 
                GUARDA_DIAMETRO * GUARDA_ESCALA,    
                GUARDA_DIAMETRO * GUARDA_ESCALA,    
                0
            );
        }
        
        else {
            al_draw_filled_circle(gx, gy, GUARDA_RAIO * GUARDA_ESCALA, al_map_rgb(200, 60, 60));
        }
    }
    // Jogador
    const float PLAYER_HALF_SIZE = 10.0f; 
    const float PLAYER_SIZE = PLAYER_HALF_SIZE * 2.0f; 
    const float PLAYER_ESCALA = sx; 
    float px = mg5_player.x * sx;
    float py = mg5_player.y * sy;

    if (mg5_jogador_img) { 
        al_draw_scaled_bitmap(
            mg5_jogador_img,
            0, 0, al_get_bitmap_width(mg5_jogador_img), al_get_bitmap_height(mg5_jogador_img),
            px - (PLAYER_HALF_SIZE * sx), 
            py - (PLAYER_HALF_SIZE * sy), 
            PLAYER_SIZE * sx,             
            PLAYER_SIZE * sy,             
            0
        );
    }
    else {
        al_draw_filled_rectangle(px - 10 * sx, py - 10 * sy, px + 10 * sx, py + 10 * sy, al_map_rgb(0, 150, 255));
    }
    char hud[64];
    snprintf(hud, sizeof(hud), "Tempo: %d   Restantes: %d", (int)ceilf(mg5_remaining), left);
    al_draw_text(fonte_builtin, al_map_rgb(240, 240, 240), 8, 6, 0, hud);
    al_draw_text(fonte_builtin, al_map_rgb(220, 220, 120), 8, 28, 0, "Dica: mova com WASD. Recolha todas as mensagens e vá até a saída verde.");

    // Mensagens
    if (mg5_game_over) {
        if (mg5_win) {
            al_draw_text(fonte_builtin, al_map_rgb(0, 200, 80), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTRE, "Você completou a rede de apoio! Indo para próxima missão...");
        }
        else {
            al_draw_text(fonte_builtin, al_map_rgb(255, 80, 80), LARGURA / 2, ALTURA / 2 - 10, ALLEGRO_ALIGN_CENTRE, "Você foi descoberto!");
            al_draw_text(fonte_builtin, al_map_rgb(220, 220, 220), LARGURA / 2, ALTURA / 2 + 20, ALLEGRO_ALIGN_CENTRE, "Pressione ENTER e volte para a prisão.");
        }
    }
}

// -------------------- MG6 --------------------

void mg6_reset(void) {
    mg6_scale_x = (float)LARGURA / MG6_BASE_W;
    mg6_scale_y = (float)ALTURA / MG6_BASE_H;
    mg6_initialized = true;

    mg6_barraX = (LARGURA - mg6_barraLargura * mg6_scale_x) - mg6_margem * mg6_scale_x;
    mg6_barraY = (ALTURA - mg6_barraAltura * mg6_scale_y) - mg6_margem * mg6_scale_y;
    mg6_indicadorX = mg6_barraX;
    mg6_velocidade = 6.0f * mg6_scale_x;
    mg6_zonaVerdeInicio = mg6_barraX + (mg6_barraLargura * mg6_scale_x / 2.0f) - 10 * mg6_scale_x;
    mg6_zonaVerdeFim = mg6_barraX + (mg6_barraLargura * mg6_scale_x / 2.0f) + 10 * mg6_scale_x;
    mg6_heliX = LARGURA / 2.0f;
    mg6_heliY = 100.0f * mg6_scale_y;
    mg6_personagemX = LARGURA / 2.0f;
    mg6_chaoY = ALTURA - 20.0f * mg6_scale_y;
    mg6_personagemAltura = 45.0f * mg6_scale_y;
    mg6_personagemY = mg6_chaoY - mg6_personagemAltura;
    mg6_jogando = true; mg6_ganhou = false; mg6_perdeu = false;
    mg6_finished = false; mg6_finish_timer = 0.0;
}

EstadoJogo mg6_logica(ALLEGRO_EVENT* ev) {
    if (!mg6_initialized) mg6_reset();

    if (ev->type == ALLEGRO_EVENT_TIMER) {
        if (mg6_jogando) {
            mg6_indicadorX += mg6_velocidade;
            if (mg6_indicadorX > mg6_barraX + mg6_barraLargura * mg6_scale_x - 10 * mg6_scale_x || mg6_indicadorX < mg6_barraX)
                mg6_velocidade *= -1;
        }
        else {
            if (mg6_ganhou && mg6_heliY < mg6_personagemY - 150.0f * mg6_scale_y)
                mg6_heliY += 2.0f * mg6_scale_y;
            if (mg6_perdeu && mg6_heliY > -100.0f * mg6_scale_y)
                mg6_heliY -= 4.0f * mg6_scale_y;
        }

        if (mg6_finished && mg6_ganhou) {
            mg6_finish_timer += 1.0 / 60.0;
            if (mg6_finish_timer >= 1.5) {
                setup_personagem(&jogador);
                return ESTADO_TELA_VOCE_GANHOU;
            }
        }
    }

    else if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
        // Espaço
        if (ev->keyboard.keycode == ALLEGRO_KEY_SPACE && mg6_jogando) {
            if (mg6_indicadorX >= mg6_zonaVerdeInicio && mg6_indicadorX <= mg6_zonaVerdeFim) {
                mg6_ganhou = true;
                mg6_jogando = false;
                mg6_finished = true;
                mg6_finish_timer = 0.0;
            }
            else {
                mg6_perdeu = true;
                mg6_jogando = false;
                mg6_finished = false; 
            }
        }

        else if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER && mg6_perdeu) {
            return ESTADO_MAPA_PRISAO;
        }
    }

    else if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return ESTADO_INICIAL;
    }

    return ESTADO_MINIGAME6;
}


void mg6_draw(void) {
    if (!mg6_initialized) mg6_reset();

    al_clear_to_color(al_map_rgb(10, 15, 25));
    al_draw_filled_rectangle(0, ALTURA - 40, LARGURA, ALTURA, al_map_rgb(30, 60, 30));
    // Helicóptero
    const float HELI_LARGURA = 80.0f; 
    const float HELI_ALTURA = 50.0f;  

    float hx = mg6_heliX;
    float hy = mg6_heliY;

    if (mg6_heli_img) { 
        al_draw_scaled_bitmap(
            mg6_heli_img,
            0, 0, al_get_bitmap_width(mg6_heli_img), al_get_bitmap_height(mg6_heli_img),
            hx - (HELI_LARGURA * mg6_scale_x / 2.0f), 
            hy,                                       
            HELI_LARGURA * mg6_scale_x,               
            HELI_ALTURA * mg6_scale_y,                
            0
        );
    }
    else {
        al_draw_filled_rectangle(hx - 40 * mg6_scale_x, hy, hx + 40 * mg6_scale_x, hy + 20 * mg6_scale_y, al_map_rgb(100, 100, 100));
        al_draw_filled_rectangle(hx - 10 * mg6_scale_x, hy - 10 * mg6_scale_y, hx + 10 * mg6_scale_x, hy, al_map_rgb(150, 150, 150));
        al_draw_line(hx - 50 * mg6_scale_x, hy, hx + 50 * mg6_scale_x, hy, al_map_rgb(200, 200, 200), 3 * mg6_scale_x);
        al_draw_filled_rectangle(hx - 5 * mg6_scale_x, hy + 20 * mg6_scale_y, hx + 5 * mg6_scale_x, hy + 35 * mg6_scale_y, al_map_rgb(80, 80, 80));
    }
    // personagem
    al_draw_filled_circle(mg6_personagemX, mg6_personagemY - 20, 10, al_map_rgb(255, 220, 180));
    al_draw_filled_rectangle(mg6_personagemX - 8, mg6_personagemY - 20, mg6_personagemX + 8, mg6_personagemY + 10, al_map_rgb(0, 100, 200));
    al_draw_filled_rectangle(mg6_personagemX - 8, mg6_personagemY + 10, mg6_personagemX - 2, mg6_personagemY + 25, al_map_rgb(0, 100, 200));
    al_draw_filled_rectangle(mg6_personagemX + 2, mg6_personagemY + 10, mg6_personagemX + 8, mg6_personagemY + 25, al_map_rgb(0, 100, 200));

    // Mensagem
    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, 50, ALLEGRO_ALIGN_CENTRE, "Extração por Helicóptero - Pressione ESPAÇO no momento certo!");

    // barra
    al_draw_filled_rectangle(mg6_barraX, mg6_barraY, mg6_barraX + mg6_barraLargura * mg6_scale_x, mg6_barraY + mg6_barraAltura * mg6_scale_y, al_map_rgb(60, 60, 60));
    al_draw_filled_rectangle(mg6_zonaVerdeInicio, mg6_barraY, mg6_zonaVerdeFim, mg6_barraY + mg6_barraAltura * mg6_scale_y, al_map_rgb(0, 200, 0));
    // indicador
    al_draw_filled_rectangle(mg6_indicadorX, mg6_barraY, mg6_indicadorX + 10 * mg6_scale_x, mg6_barraY + mg6_barraAltura * mg6_scale_y, al_map_rgb(255, 0, 0));

    // Mensagens
    if (mg6_ganhou) {
        al_draw_text(fonte_builtin, al_map_rgb(0, 255, 0), LARGURA / 2, ALTURA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "Acerto perfeito! Helicóptero pousando com sucesso!");
        al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2 - 10, ALLEGRO_ALIGN_CENTRE, "Você escapou com os documentos!");
    }
    if (mg6_perdeu) {
        al_draw_text(fonte_builtin, al_map_rgb(255, 0, 0), LARGURA / 2, ALTURA / 2 - 40, ALLEGRO_ALIGN_CENTRE, "Sinal detectado! Tropas a caminho!");
        al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2 - 10, ALLEGRO_ALIGN_CENTRE, "Missão falhou, pressione ENTER e retorne a prisão.");
    }
}

// -------------------- loop --------------------
int inicializar_allegro() {
    if (!al_init()) return 0;
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(LARGURA, ALTURA);
    if (!display) return 0;
    event_queue = al_create_event_queue();
    if (!event_queue) return 0;
    timer_global = al_create_timer(1.0 / 60.0);
    if (!timer_global) return 0;

    fonte_builtin = al_create_builtin_font();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer_global));
    return 1;
}

void game_loop() {
    bool done = false;
    al_start_timer(timer_global);
    bool redraw = true;

    // MG3 e MG4
    mg3_completed = false;
    mg4_ativado = false;

    // MG5
    bool mg5_active = false;

    while (!done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { done = true; }

        EstadoJogo proximo_estado = estado_atual;

        
        switch (estado_atual) {
        case ESTADO_INICIAL:
            proximo_estado = logica_tela_inicial(&ev);
            break;
        case ESTADO_COMO_JOGAR: 
            proximo_estado = logica_tela_como_jogar(&ev);
            break;
        case ESTADO_CONTEXTUALIZACAO:
        case ESTADO_ALERTA:
            proximo_estado = logica_transicao_por_toque(estado_atual, &ev);
            break;
        case ESTADO_INTERROGATORIO:
            proximo_estado = logica_tela_interrogatorio(&ev);
            break;
        case ESTADO_INTERROGATORIO_2:
            proximo_estado = logica_tela_interrogatorio_2(&ev);
            break;
        case ESTADO_MAPA_PRISAO:
            proximo_estado = logica_mapa_prisao(&ev);
            break;
        case ESTADO_MAPA_BRASIL:
            atualiza_personagem(&jogador, &ev);
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (distf(jogador.x, jogador.y, TRG5_X, TRG5_Y) < TRG5_RAIO) {
                    mg5_reset();
                    proximo_estado = ESTADO_MINIGAME5;
                }
            }
            break;
        case ESTADO_MAPA_EXILIO:
            atualiza_personagem(&jogador, &ev);
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (!mg3_completed) {
                    if (distf(jogador.x, jogador.y, TRG3_X, TRG3_Y) < TRG3_RAIO) {
                        mg3_reset();
                        proximo_estado = ESTADO_MINIGAME3;
                    }
                }
                else if (mg4_ativado) {
                    if (distf(jogador.x, jogador.y, TRG4_X, TRG4_Y) < TRG4_RAIO) {
                        mg4_reset();
                        proximo_estado = ESTADO_MINIGAME4;
                    }
                }
            }
            break;
        case ESTADO_MINIGAME1:
            proximo_estado = mg1_logica(&ev);
            break;
        case ESTADO_MINIGAME2:
            proximo_estado = mg2_logica(&ev);
            break;
        case ESTADO_MINIGAME3:
            proximo_estado = mg3_logica(&ev);
            break;
        case ESTADO_MINIGAME4:
            proximo_estado = mg4_logica(&ev);
            break;
        case ESTADO_MINIGAME5:
            proximo_estado = mg5_logica(&ev);
            break;
        case ESTADO_MINIGAME6:
            proximo_estado = mg6_logica(&ev);
        case ESTADO_TELA_VOCE_GANHOU:
            break;
        default:
            break;
        }

        if (proximo_estado != estado_atual) {
            estado_atual = proximo_estado;
            if (estado_atual == ESTADO_MINIGAME1) mg1_reset();
            if (estado_atual == ESTADO_MINIGAME2) mg2_reset();
            if (estado_atual == ESTADO_MINIGAME3) mg3_reset();
            if (estado_atual == ESTADO_MINIGAME4) mg4_reset();
            if (estado_atual == ESTADO_MINIGAME5) mg5_reset();
            if (estado_atual == ESTADO_MINIGAME6) mg6_reset();
            redraw = true;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) redraw = true;

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            switch (estado_atual) {
            case ESTADO_INICIAL:
                desenha_tela_inicial();
                break;
            case ESTADO_COMO_JOGAR: 
                desenha_tela_como_jogar();
                break;
            case ESTADO_CONTEXTUALIZACAO:
                desenha_tela_contextualizacao();
                break;
            case ESTADO_ALERTA:
                desenha_tela_alerta();
                break;
            case ESTADO_INTERROGATORIO:
                desenha_tela_interrogatorio();
                break;
            case ESTADO_INTERROGATORIO_2:
                desenha_tela_interrogatorio_2();
                break;
            case ESTADO_MAPA_BRASIL:
                if (fundo_mapa_brasil) al_draw_bitmap(fundo_mapa_brasil, 0, 0, 0);
                else al_clear_to_color(al_map_rgb(30, 60, 30));
                al_draw_filled_circle(TRG5_X, TRG5_Y, TRG5_RAIO, al_map_rgba_f(0.6, 0.4, 0.1, 0.4));
                al_draw_textf(fonte_builtin, al_map_rgb(255, 255, 255), TRG5_X, TRG5_Y - TRG5_RAIO - 20, ALLEGRO_ALIGN_CENTRE, "MiniGame 5");
                desenha_personagem(&jogador);
                break;
            case ESTADO_MAPA_PRISAO:
                desenha_mapa_prisao();
                break;
            case ESTADO_MINIGAME1:
                mg1_draw();
                break;
            case ESTADO_MINIGAME2:
                mg2_draw();
                break;
            case ESTADO_MAPA_EXILIO:
                if (fundo_mapa_exilio) al_draw_bitmap(fundo_mapa_exilio, 0, 0, 0);
                else {
                    al_clear_to_color(al_map_rgb(25, 40, 80));
                    al_draw_text(fonte_builtin, al_map_rgb(255, 255, 255), LARGURA / 2, 40, ALLEGRO_ALIGN_CENTRE, "Mapa do Exílio");
                }
                if (!mg3_completed) {
                    al_draw_filled_circle(TRG3_X, TRG3_Y, TRG3_RAIO, al_map_rgba_f(1.0, 0.5, 0.2, 0.6));
                    al_draw_textf(fonte_builtin, al_map_rgb(255, 255, 255), TRG3_X, TRG3_Y - TRG3_RAIO - 20, ALLEGRO_ALIGN_CENTRE, "MiniGame 3");
                }
                else if (mg4_ativado) {
                    al_draw_filled_circle(TRG4_X, TRG4_Y, TRG4_RAIO, al_map_rgba_f(0.2, 0.7, 1.0, 0.6));
                    al_draw_textf(fonte_builtin, al_map_rgb(255, 255, 255), TRG4_X, TRG4_Y - TRG4_RAIO - 20, ALLEGRO_ALIGN_CENTRE, "MiniGame 4");
                }
                desenha_personagem(&jogador);
                break;
            case ESTADO_MINIGAME3:
                mg3_draw();
                break;
            case ESTADO_MINIGAME4:
                mg4_draw();
                break;
            case ESTADO_MINIGAME5:
                mg5_draw();
                break;
            case ESTADO_MINIGAME6:
                mg6_draw();
                break;
            case ESTADO_TELA_VOCE_GANHOU:
                desenha_tela_voce_ganhou();
            }
            al_flip_display();
        }
    }
}

// -------------------- main --------------------
int main() {
    if (!inicializar_allegro()) {
        fprintf(stderr, "Falha ao inicializar Allegro\n");
        return 1;
    }
    carrega_imagens();
    setup_personagem(&jogador);
    game_loop();
    destruir_recursos();
    return 0;
}
