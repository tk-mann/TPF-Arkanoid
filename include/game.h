#ifndef GAME_H_
#define GAME_H_

#include "game_struct.h"

void actualizar_bala(BALL *ball);
int detectar_colisiones(GAME_STATE *estado_juego, BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks, int width, int height, int top);
void detectar_condiciones(GAME_STATE* estado,  BALL *ball, PLAYER *player, BLOCK **blocks, int num_blocks, int height, int width,int top);
void controlar_velocidad(double* timer, int *num_blocks, BALL* ball);
void gestionar_endgame(GAME_STATE *estado_juego);

#endif /* GAME_H_ */

