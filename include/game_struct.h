#ifndef GAME_STRUCT_H_
#define GAME_STRUCT_H_

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
	int x;
	int y;
} BALL;

typedef struct{
	int x;
	int y;
	int type;
	bool alive;
} BLOCK;

#endif /* GAME_STRUCT_H_ */