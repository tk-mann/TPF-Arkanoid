#ifndef GAME_STRUCT_H_
#define GAME_STRUCT_H_
#include <stdbool.h>
enum ESTADOS{DERROTA, VICTORIA, JUGAR, SALIR, REINICIAR, FIN};
enum ESTADOS_BALA{PLAY, RESET, WAIT};
enum BLOCK_TYPE{NORMAL, HEAVY, UNBREAKABLE};
typedef struct{
	int state;
	int points;
	int level;
	int vidas;
	int dead_blocks;
    int block_counter;
    double timer, prev_timer;
} GAME_STATE;
typedef struct{
	float x;   // cambiado de int a float para acumulación precisa con delta time
	float y;   // cambiado de int a float para acumulación precisa con delta time
    int width;
    int height;
} PLAYER;
typedef struct{
	float x, y;
    float size;
    float prev_x, prev_y;
    int state;
    float vx, vy;
    float speed_factor;
    float speed;
} BALL;
typedef struct{
	float x, y;
	int width, height;
	int dureza;
	bool alive;
    int type;
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