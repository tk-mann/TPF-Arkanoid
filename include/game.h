#ifndef GAME_H_
#define GAME_H_

#include "game_struct.h"

void actualizar_bala(BALL *ball);
int detectar_colisiones(BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks);

#endif /* GAME_H_ */