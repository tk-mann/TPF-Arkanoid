#ifndef FRONTEND_H_
#define FRONTEND_H_

#define ALLEGRO_W  512
#define ALLEGRO_H 512
#define FPS 60
enum ESTADOS{DERROTA, VICTORIA, JUGAR, SALIR, REINICIAR};

#include "game_struct.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> //manejo de imágenes
#include <allegro5/allegro_primitives.h> //manejo de formas geométricas
#include <allegro5/allegro_font.h> //manejo de fonts
#include <allegro5/allegro_ttf.h> //Manejo de ttfs
#include <allegro5/allegro_color.h> //Manejo de colores
#include <time.h>

int menu();
int init(GAME_STATE *estado_juego);

void play(GAME_STATE *estado_juego);
void end_phase(GAME_STATE *estado_juego);
void de_init();

#endif /* FRONTEND_H_ */