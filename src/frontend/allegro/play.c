#include "frontend.h"
#include "game.h"
#include "game_struct.h"
#include <allegro5/allegro_color.h>
#include "backend.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


// Variables externas
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_EVENT event;
extern ALLEGRO_FONT * font;
ALLEGRO_TIMER *timer = NULL;

#define BLOCK_WIDTH  (ALLEGRO_W / 11.0f)
#define BLOCK_HEIGHT 16.0f 
#define LVL_1_BLOCKS 66
#define LVL_2_BLOCKS 66 
#define PLAYER_START_X (ALLEGRO_W / 2 - 50)
#define PLAYER_START_Y (ALLEGRO_H - 50)
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 20

enum KEYS { KEY_LEFT=0, KEY_RIGHT, KEY_SPACE, KEY_ESCAPE };

// Velocidades en píxeles por SEGUNDO (no por frame)
#define NAVE_MOVE_RATE  300.0f
#define BALL_INIT_SPEED 300.0f   // px/seg — ajustá este valor a tu gusto

// Estado de teclas
static bool key_pressed[4] = {false, false, false, false};

typedef struct{
	BLOCK *block;
    ALLEGRO_BITMAP **block_bitmaps;
    int num_blocks;
} BLOCK_ARRANGE;

typedef struct{
	ALLEGRO_BITMAP *player_bitmap;
    PLAYER player;
} ALL_PLAYER;

typedef struct{
	ALLEGRO_BITMAP *ball_bitmap;
    BALL ball;
} ALL_BALL;

void indicadores(GAME_STATE *estado_juego) {
    char puntos_text[20];
    char vidas_text[20];
    char nivel_text[20];

    sprintf(puntos_text, "Puntos: %d", estado_juego->points);
    sprintf(vidas_text, "Vidas: %d", estado_juego->vidas);
    sprintf(nivel_text, "Nivel: %d", estado_juego->level);

    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, puntos_text);
    al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W/2 - al_get_text_width(font, vidas_text)/2, 10, 0, vidas_text);
    al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W - 100, 10, 0, nivel_text);
}

BLOCK_ARRANGE create_block_arrangement(int level){
    BLOCK_ARRANGE p;
    int size;
    switch(level){
        case 1:
            size = LVL_1_BLOCKS;
            break;
        case 2:
            size = LVL_2_BLOCKS;
            break;
        default:
            size = LVL_1_BLOCKS;
            break;
    }
    p.block_bitmaps = malloc(sizeof(ALLEGRO_BITMAP*)*size);
    p.block = malloc(sizeof(BLOCK)*size);
    p.num_blocks = size;
    return p;
}

void actualizar_bloques(BLOCK_ARRANGE *arrangement, int num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        if (arrangement->block[i].alive) {
            al_draw_bitmap(arrangement->block_bitmaps[i], arrangement->block[i].x, arrangement->block[i].y, 0);
        }
    }
}

void actualizar_movimientos(ALL_PLAYER *player, ALL_BALL *ball, BLOCK_ARRANGE *arrangement) {
    al_draw_bitmap(ball->ball_bitmap, ball->ball.x, ball->ball.y, 0);
    al_draw_bitmap(player->player_bitmap, player->player.x, player->player.y, 0);
    al_draw_line(0, TOP_PADDING, ALLEGRO_W, TOP_PADDING, al_map_rgb(255, 255, 255), 2);
    actualizar_bloques(arrangement, arrangement->num_blocks);
}

static void registrar_teclas() {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            key_pressed[KEY_LEFT] = true;
            break;
        case ALLEGRO_KEY_RIGHT:
            key_pressed[KEY_RIGHT] = true;
            break;
        case ALLEGRO_KEY_SPACE:
            key_pressed[KEY_SPACE] = true;
            break;
        case ALLEGRO_KEY_ESCAPE:
            key_pressed[KEY_ESCAPE] = true;
            break;
        }
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_LEFT:
            key_pressed[KEY_LEFT] = false;
            break;
        case ALLEGRO_KEY_RIGHT:
            key_pressed[KEY_RIGHT] = false;
            break;
        case ALLEGRO_KEY_SPACE:
            key_pressed[KEY_SPACE] = false;
            break;
        case ALLEGRO_KEY_ESCAPE:
            key_pressed[KEY_ESCAPE] = false;
            break;
        }
    }
}

// dt: delta time en segundos desde el último frame
void procesar_entradas(GAME_STATE *estado_juego, ALL_PLAYER *player, ALL_BALL *ball, float dt) {
    if (key_pressed[KEY_LEFT]) {
        float nuevo_x = player->player.x - NAVE_MOVE_RATE * dt;
        if (nuevo_x >= 0) {
            player->player.x = nuevo_x;
            if (ball->ball.state == WAIT) {
                ball->ball.x = nuevo_x + player->player.width / 2.0f - ball->ball.size / 2.0f;
            }
        }
    } else if (key_pressed[KEY_RIGHT]) {
        float nuevo_x = player->player.x + NAVE_MOVE_RATE * dt;
        if (nuevo_x + PLAYER_WIDTH <= ALLEGRO_W) {
            player->player.x = nuevo_x;
            if (ball->ball.state == WAIT) {
                ball->ball.x = nuevo_x + player->player.width / 2.0f - ball->ball.size / 2.0f;
            }
        }
    }

    if (key_pressed[KEY_SPACE] && ball->ball.state == WAIT) {
        ball->ball.state = PLAY;
        // Velocidad inicial en px/seg a 45°
        ball->ball.vx = -BALL_INIT_SPEED * 0.7071f;  // cos(45°)
        ball->ball.vy = -BALL_INIT_SPEED * 0.7071f;  // sin(45°)
        ball->ball.speed = BALL_INIT_SPEED;
        ball->ball.speed_factor = 1.0f;
    }

    if (key_pressed[KEY_ESCAPE]) {
        al_stop_timer(timer);
        estado_juego->state = menu_pausa();
        al_start_timer(timer);
        key_pressed[KEY_ESCAPE] = false;
    }
}


static ALLEGRO_COLOR get_color_from_enum(BlockColor color) {
    switch (color) {
        case COLOR_RED:    return al_map_rgb(255, 0, 0);
        case COLOR_ORANGE: return al_map_rgb(255, 165, 0);
        case COLOR_YELLOW: return al_map_rgb(255, 255, 0);
        case COLOR_GREEN:  return al_map_rgb(0, 255, 0);
        case COLOR_CYAN:   return al_map_rgb(0, 255, 255);
        case COLOR_BLUE:   return al_map_rgb(0, 0, 255);
        case COLOR_PURPLE: return al_map_rgb(128, 0, 128);
        case COLOR_PINK:   return al_map_rgb(255, 192, 203);
        case COLOR_WHITE:  return al_map_rgb(255, 255, 255);
        case COLOR_GRAY:   return al_map_rgb(128, 128, 128);
        default:           return al_map_rgb(255, 0, 0);
    }
}

ALLEGRO_BITMAP* create_ball_bitmap(float radius, ALLEGRO_COLOR color) {
    int size = (int)(radius * 2 + 2);
    ALLEGRO_BITMAP *ball = al_create_bitmap(size, size);
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    al_set_target_bitmap(ball);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_filled_circle(radius + 1, radius + 1, radius, color);
    al_set_target_bitmap(old_target);
    return ball;
}

static ALLEGRO_BITMAP* create_block_bitmap(BlockColor color, int width, int height) {
    ALLEGRO_BITMAP *block = al_create_bitmap(width, height);
    if (!block) return NULL;
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    al_set_target_bitmap(block);
    ALLEGRO_COLOR main_color = get_color_from_enum(color);
    al_clear_to_color(main_color);
    float r, g, b, a;
    al_unmap_rgba_f(main_color, &r, &g, &b, &a);
    ALLEGRO_COLOR dark = al_map_rgb_f(r * 0.6f, g * 0.6f, b * 0.6f);
    al_draw_rectangle(0, 0, width - 1, height - 1, dark, 1);
    ALLEGRO_COLOR light = al_map_rgb_f(
        (r * 1.3f > 1.0f ? 1.0f : r * 1.3f),
        (g * 1.3f > 1.0f ? 1.0f : g * 1.3f),
        (b * 1.3f > 1.0f ? 1.0f : b * 1.3f)
    );
    al_draw_line(1, 1, width - 2, 1, light, 1);
    al_draw_line(1, 1, 1, height - 2, light, 1);
    al_set_target_bitmap(old_target);
    return block;
}


int init_level_1(BLOCK_ARRANGE *arrangement) {
    //creacion mapa 1
    const int rows = 6;
    const int cols = 11;
    const float start_y = 30.0f + TOP_PADDING;

    BlockColor row_colors[6] = {
        COLOR_GRAY, COLOR_RED, COLOR_BLUE,
        COLOR_ORANGE, COLOR_PINK, COLOR_GREEN
    };

    int index = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (index >= LVL_1_BLOCKS) break;
            float x = c * BLOCK_WIDTH;
            float y = start_y + r * BLOCK_HEIGHT;
            arrangement->block[index].x = (int)x;
            arrangement->block[index].y = (int)y;
            arrangement->block[index].width = (int)BLOCK_WIDTH;
            arrangement->block[index].height = (int)BLOCK_HEIGHT;
            arrangement->block[index].alive = true;
            arrangement->block[index].dureza = (row_colors[r] == COLOR_GRAY) ? 2 : 1;
            switch(arrangement->block[index].dureza) {
                case 1:  arrangement->block[index].type = NORMAL; break;
                case 2:  arrangement->block[index].type = HEAVY;  break;
                case -1: arrangement->block[index].type = UNBREAKABLE; break;
            }
            arrangement->block_bitmaps[index] = create_block_bitmap(row_colors[r], (int)BLOCK_WIDTH, (int)BLOCK_HEIGHT);
            al_draw_bitmap(arrangement->block_bitmaps[index], x, y, 0);
            index++;
        }
    }
    return 0;
}

int init_level_2(BLOCK_ARRANGE *arrangement) {
    const int cols = 11;
    const float start_y = 30.0f + TOP_PADDING;
    BlockColor diagonal_colors[5] = {
        COLOR_ORANGE, COLOR_CYAN, COLOR_GREEN, COLOR_BLUE, COLOR_RED
    };
    int index = 0;
    int desplazamiento = 0;
    for (int c = 0; c < cols; c++) {
        int blocks_in_row = 11 - c;
        for (int r = 0; r < blocks_in_row; r++) {
            if (index >= LVL_1_BLOCKS) break;
            float x = c * BLOCK_WIDTH;
            float y = start_y + r * BLOCK_HEIGHT + desplazamiento;
            arrangement->block[index].x = (int)x;
            arrangement->block[index].y = (int)y;
            arrangement->block[index].width = (int)BLOCK_WIDTH;
            arrangement->block[index].height = (int)BLOCK_HEIGHT;
            arrangement->block[index].alive = true;
            if (r + 1 == blocks_in_row && c != 10) {
                arrangement->block[index].dureza = 3;
                arrangement->block[index].type = HEAVY;
                arrangement->block_bitmaps[index] = create_block_bitmap(COLOR_GRAY, (int)BLOCK_WIDTH, (int)BLOCK_HEIGHT);
            } else {
                arrangement->block[index].dureza = 2;
                arrangement->block[index].type = HEAVY;
                int color_index = c % 5;
                arrangement->block_bitmaps[index] = create_block_bitmap(diagonal_colors[color_index], (int)BLOCK_WIDTH, (int)BLOCK_HEIGHT);
            }
            al_draw_bitmap(arrangement->block_bitmaps[index], x, y, 0);
            index++;
        }
        desplazamiento += BLOCK_HEIGHT;
    }
    return 0;
}


void load_game(GAME_STATE *estado_juego, ALL_PLAYER *player, ALL_BALL *ball, BLOCK_ARRANGE *arrangement) {
    estado_juego->dead_blocks = 0;
    estado_juego->block_counter = 0;
    al_clear_to_color(al_map_rgb(0, 0, 0));
    player->player_bitmap = create_block_bitmap(COLOR_ORANGE, PLAYER_WIDTH, PLAYER_HEIGHT);
    player->player.width = PLAYER_WIDTH;
    player->player.height = PLAYER_HEIGHT;
    player->player.x = PLAYER_START_X;
    player->player.y = PLAYER_START_Y;
    al_draw_bitmap(player->player_bitmap, PLAYER_START_X, PLAYER_START_Y, 0);
    ball->ball_bitmap = create_ball_bitmap(8, al_map_rgb(255, 255, 255));
    ball->ball.size = 16;
    ball->ball.state = WAIT;
    ball->ball.speed = BALL_INIT_SPEED;
    ball->ball.speed_factor = 1.0f;
    ball->ball.x = PLAYER_START_X + PLAYER_WIDTH / 2.0f - ball->ball.size / 2.0f;
    ball->ball.y = PLAYER_START_Y - ball->ball.size;
    ball->ball.vx = 0;
    ball->ball.vy = 0;
    al_draw_bitmap(ball->ball_bitmap, ball->ball.x, ball->ball.y, 0);
    switch(estado_juego->level) {
        case 1: init_level_1(arrangement); break;
        case 2: init_level_2(arrangement); break;
    }
    al_flip_display();
}


void cleanup_play_resources(ALL_PLAYER *player, ALL_BALL *ball, BLOCK_ARRANGE *arrangement, ALLEGRO_TIMER *timer) {
    if (timer) {
        al_unregister_event_source(event_queue, al_get_timer_event_source(timer));
        al_destroy_timer(timer);
    }
    if (player->player_bitmap) {
        al_destroy_bitmap(player->player_bitmap);
        player->player_bitmap = NULL;
    }
    if (ball->ball_bitmap) {
        al_destroy_bitmap(ball->ball_bitmap);
        ball->ball_bitmap = NULL;
    }
    for (int i = 0; i < arrangement->num_blocks; i++) {
        if (arrangement->block_bitmaps[i]) {
            al_destroy_bitmap(arrangement->block_bitmaps[i]);
            arrangement->block_bitmaps[i] = NULL;
        }
    }
    for (int i = 0; i < 4; i++) key_pressed[i] = false;
    free(arrangement->block_bitmaps);
    free(arrangement->block);
    arrangement->block_bitmaps = NULL;
    arrangement->block = NULL;
}


void play(GAME_STATE *estado_juego) {
    ALL_PLAYER player = {0};
    ALL_BALL ball = {0};
    BLOCK_ARRANGE arrangement = create_block_arrangement(estado_juego->level);

    load_game(estado_juego, &player, &ball, &arrangement);

    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        cleanup_play_resources(&player, &ball, &arrangement, NULL);
        estado_juego->state = SALIR;
        return;
    }
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    // Delta time: guardamos el momento del último frame
    double last_time = al_get_time();

    while (estado_juego->state == JUGAR) {
        // wait_for_event: bloquea hasta el próximo tick del timer (correcto)
        al_get_next_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
            registrar_teclas();

        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            // --- Calcular delta time real ---
            double now = al_get_time();
            float dt = (float)(now - last_time);
            last_time = now;

            // Clamp: evita saltos si el juego se congela momentáneamente
            if (dt > 0.05f) dt = 0.05f;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Actualizar timer del juego (para controlar_velocidad)
            estado_juego->timer = al_get_timer_count(timer) * al_get_timer_speed(timer) - estado_juego->prev_timer;

            procesar_entradas(estado_juego, &player, &ball, dt);
            controlar_velocidad(&estado_juego->timer, &estado_juego->block_counter, estado_juego->level, &ball.ball);
            detectar_condiciones(estado_juego, &ball.ball, &player.player, &arrangement.block, arrangement.num_blocks, ALLEGRO_H, ALLEGRO_W, TOP_PADDING);
            actualizar_movimientos(&player, &ball, &arrangement);
            indicadores(estado_juego);
            actualizar_bala(&ball.ball, dt);

            al_flip_display();
        }
    }

    gestionar_endgame(estado_juego);
    cleanup_play_resources(&player, &ball, &arrangement, timer);
}