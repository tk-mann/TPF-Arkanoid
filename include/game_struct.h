#ifndef GAME_STRUCT_H_
#define GAME_STRUCT_H_
#include <stdbool.h>

typedef struct{
	int state;
	int points;
	int level;
	int vidas;
} GAME_STATE;

typedef struct{
	int x;
	int y;
} PLAYER;

typedef struct{
	float x, y;
    int size;
    float prev_x, prev_y; // Para almacenar la posición anterior
    bool start;
    float vx, vy;
} BALL;

typedef struct{
	int x, y;
	int width, height;
	int type;
	bool alive;
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


// Modificar la estructura Block
typedef struct {
    float x, y;
    float width, height;
	int dureza;
    BlockColor color;  // ← AGREGAR ESTO
    int hits_remaining;
    bool active;
} Block;





#endif /* GAME_STRUCT_H_ */