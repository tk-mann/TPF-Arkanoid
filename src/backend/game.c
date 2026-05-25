#include "game.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265f
#define DEG_TO_RAD(angle) ((angle) * (PI / 180.0f))

enum CARA { LEFT, RIGHT, UP, DOWN, LEFT_CORNER, RIGHT_CORNER, LEFT_DOWN_CORNER, RIGHT_DOWN_CORNER };


// Mueve la pelota según su velocidad (px/seg) escalada por dt (segundos)
void actualizar_bala(BALL *ball, float dt) {
    ball->prev_x = ball->x;
    ball->prev_y = ball->y;
    ball->x += ball->vx * dt;
    ball->y += ball->vy * dt;
}


// Determina en qué cara del bloque impactó la pelota usando su posición anterior
int determinar_cara(BALL *ball, float x, float y, int width, int height) {
    if (ball->prev_y + ball->size <= y) {
        ball->y = y - ball->size;
        return UP;
    }
    else if (ball->prev_y >= y + height) {
        ball->y = y + height;
        return DOWN;
    }
    else if (ball->prev_x + ball->size <= x) {
        ball->x = x - ball->size;
        return LEFT;
    }
    else if (ball->prev_x >= x + width) {
        ball->x = x + width;
        return RIGHT;
    } else {
        if ((ball->prev_x < x && ball->prev_y < y) || (ball->prev_x < x && ball->prev_y > y + height)) {
            return LEFT_CORNER;
        } else {
            return RIGHT_CORNER;
        }
    }
}


// Detecta y resuelve colisiones. Las velocidades son px/seg; no dependen del FPS.
int detectar_colisiones(GAME_STATE *estado_juego, BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks, int width, int height, int top) {
    // Pared izquierda
    if (ball->x <= 0) {
        ball->x = 0;
        ball->vx = -ball->vx;
    }
    // Pared derecha
    else if (ball->x >= width - ball->size) {
        ball->x = width - ball->size;
        ball->vx = -ball->vx;
    }
    // Techo
    if (ball->y <= top) {
        ball->y = top;
        ball->vy = -ball->vy;
    }
    // Piso (vida perdida — se maneja en detectar_condiciones, pero lo dejamos por seguridad)
    if (ball->y >= height - ball->size) {
        ball->y = height - ball->size;
        ball->vy = -ball->vy;
    }

    // Colisión con bloques
    for (int i = 0; i < num_blocks; i++) {
        if (blocks[i].alive) {
            if (ball->x < blocks[i].x + blocks[i].width &&
                ball->x + ball->size > blocks[i].x &&
                ball->y < blocks[i].y + blocks[i].height &&
                ball->y + ball->size > blocks[i].y)
            {
                int cara = determinar_cara(ball, blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].height);
                if (cara == LEFT || cara == RIGHT) {
                    ball->vx = -ball->vx;
                } else if (cara == UP || cara == DOWN) {
                    ball->vy = -ball->vy;
                } else {
                    ball->vx = -ball->vx;
                    ball->vy = -ball->vy;
                }
                if (blocks[i].type != UNBREAKABLE) {
                    blocks[i].dureza--;
                    if (blocks[i].dureza <= 0) {
                        estado_juego->points += (blocks[i].type == HEAVY) ? 20 : 10;
                        blocks[i].alive = false;
                        estado_juego->dead_blocks++;
                        estado_juego->block_counter++;
                    }
                }
                return 0;
            }
        }
    }

    // Colisión con el jugador
    if (ball->x < player->x + player->width &&
        ball->x + ball->size > player->x &&
        ball->y < player->y + player->height &&
        ball->y + ball->size > player->y)
    {
        int cara = determinar_cara(ball, player->x, player->y, player->width, player->height);
        if (cara == UP) {
            // Ángulo de rebote según punto de impacto en el jugador
            float impact_point = (ball->x + ball->size / 2.0f) - (player->x + player->width / 2.0f);
            float max_impact = player->width / 2.0f;
            float normalized_impact = impact_point / max_impact; // -1 a 1
            float sign;
            float angle;
            int velocity_sign = (ball->vx >= 0) ? 1 : -1;

            if ((normalized_impact >= 0.0f && ball->vx > 0) || (normalized_impact < 0.0f && ball->vx < 0)) {
                sign = 1;
            } else {
                sign = -1;
            }

            if (fabs(normalized_impact) <= 0.1f) {
                // Centro: rebote vertical con inversión horizontal
                ball->vy = -ball->vy;
                ball->vx = -ball->vx;
            } else if (fabs(normalized_impact) <= 0.7f) {
                // Zona media: ángulo 45°
                angle = DEG_TO_RAD(45.0f);
                ball->vx = sign * velocity_sign * ball->speed * cosf(angle);
                ball->vy = -ball->speed * sinf(angle);
            } else {
                // Borde: ángulo 30° (rebote más lateral)
                angle = DEG_TO_RAD(30.0f);
                ball->vx = sign * velocity_sign * ball->speed * cosf(angle);
                ball->vy = -ball->speed * sinf(angle);
            }
        } else if (cara == LEFT || cara == LEFT_CORNER) {
            ball->x = player->x - ball->size;
            ball->vx = -ball->vx;
        } else if (cara == RIGHT || cara == RIGHT_CORNER) {
            ball->x = player->x + player->width;
            ball->vx = -ball->vx;
        }
    }
    return 0;
}


// Detecta condiciones de victoria/derrota y resetea la pelota si cae
void detectar_condiciones(GAME_STATE *estado, BALL *ball, PLAYER *player, BLOCK **blocks, int num_blocks, int height, int width, int top) {
    // La pelota cayó por el fondo
    if (ball->y + ball->size >= height) {
        estado->vidas--;
        ball->state = RESET;
        estado->block_counter = 0;
        ball->speed_factor = 1.0f;
        estado->prev_timer = estado->timer;
    }

    if (ball->state == PLAY) {
        detectar_colisiones(estado, ball, player, *blocks, num_blocks, width, height, top);
    } else if (ball->state == RESET) {
        ball->x = player->x + player->width / 2.0f - ball->size / 2.0f;
        ball->y = player->y - ball->size;
        ball->vx = 0;
        ball->vy = 0;
        ball->state = WAIT;
        ball->speed_factor = 1.0f;
    }

    if (estado->vidas <= 0) {
        estado->state = DERROTA;
    }
    if (estado->dead_blocks >= num_blocks) {
        estado->state = VICTORIA;
    }
}


// Gestiona el estado al terminar el juego (victoria, derrota, reinicio)
void gestionar_endgame(GAME_STATE *estado_juego) {
    if (estado_juego->state == VICTORIA) {
        estado_juego->level++;
        estado_juego->vidas = 3;
        estado_juego->prev_timer = 0;
        if (estado_juego->level > 2) {
            estado_juego->state = FIN;
        } else {
            estado_juego->state = JUGAR;
        }
    } else if (estado_juego->state == REINICIAR) {
        estado_juego->level = 1;
        estado_juego->vidas = 3;
        estado_juego->points = 0;
        estado_juego->prev_timer = 0;
        estado_juego->state = JUGAR;
    }
}


// Controla el factor de velocidad de la pelota según bloques destruidos y tiempo.
// Las velocidades son px/seg, así que solo actualizamos el módulo (speed) y
// recalculamos vx/vy manteniendo la dirección actual.
void controlar_velocidad(double *timer, int *num_blocks, int nivel, BALL *ball) {
    float factor1 = 1.0f, factor2 = 1.0f, factor_final;
    switch(nivel) {
        case 1:
            // Factor por bloques destruidos
            if      (*num_blocks > 60) factor1 = 1.28f;
            else if (*num_blocks > 48) factor1 = 1.24f;
            else if (*num_blocks > 36) factor1 = 1.20f;
            else if (*num_blocks > 24) factor1 = 1.16f;
            else if (*num_blocks > 12) factor1 = 1.08f;
            else if (*num_blocks >= 0) factor1 = 1.05f;

            // Factor por tiempo transcurrido
            if      (*timer >= 120.0) factor2 = 1.25f;
            else if (*timer >= 90.0)  factor2 = 1.20f;
            else if (*timer >= 60.0)  factor2 = 1.14f;
            else if (*timer >= 30.0)  factor2 = 1.06f;
            break;
        case 2:
            // Factor por bloques destruidos
            if      (*num_blocks > 60) factor1 = 1.30f;
            else if (*num_blocks > 48) factor1 = 1.29f;
            else if (*num_blocks > 36) factor1 = 1.25f;
            else if (*num_blocks > 24) factor1 = 1.21f;
            else if (*num_blocks > 12) factor1 = 1.13f;
            else if (*num_blocks >= 0) factor1 = 1.09f;

            // Factor por tiempo transcurrido
            if      (*timer >= 120.0) factor2 = 1.29f;
            else if (*timer >= 90.0)  factor2 = 1.25f;
            else if (*timer >= 60.0)  factor2 = 1.19f;
            else if (*timer >= 30.0)  factor2 = 1.10f;
            break;
    }

    factor_final = (factor1 >= factor2) ? factor1 : factor2;

    if (factor_final != ball->speed_factor && ball->state == PLAY) {
        
        ball->speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy) * factor_final ;
        ball->speed_factor = factor_final;
    }
}