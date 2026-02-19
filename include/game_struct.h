#ifndef GAME_STRUCT_H_
#define GAME_STRUCT_H_

#include <stdbool.h>

enum ESTADOS{DERROTA, VICTORIA, JUGAR, SALIR, REINICIAR};
enum ESTADOS_BALA{PLAY, RESET, WAIT};
enum BLOCK_TYPE{NORMAL, HEAVY, UNBREAKABLE};

typedef struct{
	int state;
	int points;
	int level;
	int vidas;
} GAME_STATE;

typedef struct{
	int x;
	int y;
    int width;
    int height;
} PLAYER;

typedef struct{
	float x, y;
    int size;
    float prev_x, prev_y; // Para almacenar la posición anterior
    int state;
    float vx, vy;
} BALL;

typedef struct{
	int x, y;
	int width, height;
	int dureza;
	bool alive;
    int type;
    int dead_block;
} BLOCK;

typedef enum {
    COLOR_RED = 0,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_PINK,
    COLOR_WHITE,
    COLOR_GRAY
} BlockColor;




#endif /* GAME_STRUCT_H_ */