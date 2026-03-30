#include "game.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265f
#define DEG_TO_RAD(angle) ((angle) * (PI / 180.0f))

enum CARA { LEFT, RIGHT, UP, DOWN, LEFT_CORNER, RIGHT_CORNER, LEFT_DOWN_CORNER, RIGHT_DOWN_CORNER };


//actualiza la velocidad de la bala
void actualizar_bala(BALL *ball) {
    // Actualizar posición de la pelota
    //las posiciónes se actualizan en función de la velocidad
    ball->prev_x = ball->x;
    ball->prev_y = ball->y;
    ball->x += ball->vx;
    ball->y += ball->vy;

}


//determina en que cara del bloque sufrió el disparo
int determinar_cara(BALL *ball, int x, int y, int width, int height) {
    // Resolver usando posición anterior para evitar que la pelota se meta en el objeto
    if(ball->prev_y + ball->size <= y) {
        ball->y = y - ball->size; // Ajustar posición para evitar que se meta
        return UP;
    }
    else if(ball->prev_y >= y + height) {
        ball->y = y + height; // Ajustar posición para evitar que se meta
        return DOWN;
    }
    else if(ball->prev_x + ball->size <= x) {
        ball->x = x - ball->size; // Ajustar posición para evitar que se meta
        return LEFT;
    }
    else if(ball->prev_x >= x + width) {
        ball->x = x + width; // Ajustar posición para evitar que se meta
        return RIGHT;
    }else{
        //las condiciones de esquina se tratan como condiciones laterales
        if((ball->prev_x < x && ball->prev_y < y) || (ball->prev_x < x && ball->prev_y > y + height)) {
            return LEFT_CORNER;
        } else {
            return RIGHT_CORNER;
        }
        
    }
}


//detecta colisiones entre la pelota y los objetos del juego, y actualiza el estado del juego en consecuencia
int detectar_colisiones(GAME_STATE *estado_juego, BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks, int width, int height, int top) {
    //deteccion colisión pared izquierda
    if(ball->x <= 0){
        ball->x = 0;
        ball->vx = -ball->vx;
    }
    //deteccion colisión pared derecha
    else if(ball->x >= width - ball->size){
        ball->x = width- ball->size;
        ball->vx = -ball->vx;
    }
    //deteccion colisión techo
    if(ball->y <= top){
        ball->y = top;
        ball->vy = -ball->vy;
    }
    if(ball->y >= height - ball->size){
        ball->y = height - ball->size;
        ball->vy = -ball->vy;
    }
    //detección colisión bloque
    int i;
    for(i = 0; i < num_blocks; i++){
        if(blocks[i].alive){
            if(ball->x < blocks[i].x + blocks[i].width &&
            ball->x + ball->size > blocks[i].x &&
            ball->y < blocks[i].y + blocks[i].height &&
            ball->y + ball->size > blocks[i].y){ 
                int cara = determinar_cara(ball, blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].height);
                if(cara == LEFT || cara == RIGHT){
                    ball->vx = -ball->vx;
                } else if(cara == UP || cara == DOWN) {
                    ball->vy = -ball->vy;
                } else {
                    ball->vx = -ball->vx;
                    ball->vy = -ball->vy;
                }
                if(blocks[i].type != UNBREAKABLE){
                    blocks[i].dureza--;
                    // printf("Colisión con bloque, tipo: %d, dureza restante: %d\n", blocks[i].type, blocks[i].dureza);
                    if((blocks[i].dureza) <= 0){
                        if(blocks[i].type == HEAVY){
                            estado_juego->points += 20;
                        } else {
                            estado_juego->points += 10;
                        }
                    blocks[i].alive = false;
                    estado_juego->dead_blocks++;
                    estado_juego->block_counter++;
                    }
                }
                return 0;
            }
        }
    }
    //detección colisión jugador
    if(ball->x < player->x + player->width &&
        ball->x + ball->size > player->x &&
        ball->y < player->y + player->height &&
        ball->y + ball->size > player->y){
        int cara = determinar_cara(ball, player->x, player->y, player->width, player->height);
        // printf("Colisión con jugador, cara: %d\n", cara);
        if(cara == UP){
            // Ajustar la velocidad horizontal de la pelota según el punto de impacto en el jugador
            float impact_point = (ball->x + ball->size / 2.0f) - (player->x + player->width / 2.0f);
            float max_impact = player->width / 2.0f;
            float normalized_impact = impact_point / max_impact; // Rango de -1 a 1
            float sign;
            float angle;
            int velocity_sign = (ball->vx >= 0) ? 1 : -1; // Determina el signo de la velocidad horizontal actual
            if((normalized_impact >= 0.0f && ball->vx > 0) || (normalized_impact < 0.0f && ball->vx < 0)) {
                // Invertir dirección si la pelota ya se mueve hacia el lado del impacto
                sign = 1; 
            }else{
                sign = -1;
            }
            if(fabs(normalized_impact) <= 0.1f) {
                ball->vy = -ball->vy;
                ball->vx = -ball->vx;
            }
            else if(fabs(normalized_impact) > 0.1f && fabs(normalized_impact) <= 0.7f){
                angle = DEG_TO_RAD(45.0f);
                ball->vx = sign * velocity_sign * ball->speed * cos(angle);
                ball->vy = -1 * ball->speed * sin(angle);
            } else {
                angle = DEG_TO_RAD(30.0f);
                ball->vx = sign * velocity_sign * ball->speed * cos(angle);
                ball->vy = -1 * ball->speed * sin(angle);
            }
        }else if(cara == LEFT || cara == LEFT_CORNER){
            ball->x = player->x - ball->size; // Ajustar posición para evitar que se meta
            ball->vx = -ball->vx;
        }else if(cara == RIGHT || cara == RIGHT_CORNER){
            ball->x = player->x + player->width; // Ajustar posición para evitar que se meta
            ball->vx = -ball->vx;
        }
    }
    return 0;
}


//detecta condiciones de victoria o derrota, y actualiza el estado del juego en consecuencia
void detectar_condiciones(GAME_STATE* estado,  BALL *ball, PLAYER *player, BLOCK **blocks, int num_blocks, int height, int width, int top) {
    if(ball->y + ball->size >= height) {
        estado->vidas--;
        ball->state = RESET;
        estado->block_counter = 0;
        ball->speed_factor = 1.0f;
        estado->prev_timer = estado->timer;
    }

    if(ball->state == PLAY) {
        detectar_colisiones(estado, ball, player, *blocks, num_blocks, width, height, top);
    }
    else if(ball->state == RESET) {
        ball->x = player->x + player->width / 2 - ball->size / 2;
        ball->y = player->y - ball->size;
        ball->vx = 0;
        ball->vy = 0;
        ball->state = WAIT;
        ball->speed_factor = 0.0f;
    }
    if(estado->vidas <= 0) {
        estado->state = DERROTA; // Game Over
    }
    if(estado->dead_blocks >= num_blocks) {
        estado->state = VICTORIA; // Victoria
    }

}

//gestiona el final del juego, actualizando el estado del juego para reflejar si el jugador ha ganado, perdido o reiniciado el juego
void gestionar_endgame(GAME_STATE *estado_juego) {
    if(estado_juego->state == VICTORIA){
        estado_juego->level++;
        estado_juego->vidas = 3;
        estado_juego->prev_timer = 0;
        if(estado_juego->level > 2){
            estado_juego->state = FIN;
        } else {
            estado_juego->state = JUGAR;
        }

    }  else if(estado_juego->state == REINICIAR){
        estado_juego->level = 1;
        estado_juego->vidas = 3;
        estado_juego->points = 0;
        estado_juego->prev_timer = 0;
        estado_juego->state = JUGAR;
    }

}

//controla la velocidad de la pelota a medida que avanza el juego, aumentando la velocidad en función del tiempo transcurrido y la cantidad de bloques destruidos
void controlar_velocidad(double* timer, int *num_blocks, BALL* ball) {
    float factor1 = 1, factor2 = 1, factor_final;
    if(*num_blocks > 60){
        factor1 =  1.28;
    } else if(*num_blocks > 48){
        factor1 = 1.24;
    } else if(*num_blocks > 36){
        factor1 = 1.20;
    }else if(*num_blocks > 24){
        factor1 =  1.16;
    }else if(*num_blocks > 12){
        factor1 = 1.08;
    }
    if(*timer >= 120.0){
        factor2 = 1.25;
    }else if(*timer >= 90.0){
        factor2 = 1.20;
    }else if(*timer >= 60.0){
        factor2 = 1.14;
    }else if(*timer >= 30.0){
        factor2 = 1.05;
    }
    factor_final = factor1>=factor2 ? factor1 : factor2;
    if((factor_final != ball->speed_factor) && ball->state == PLAY){
        ball->speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy) * (ball->speed_factor + (factor_final - ball->speed_factor));
        ball->speed_factor = factor_final;
    }
}
