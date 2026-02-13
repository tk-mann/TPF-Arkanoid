#include "arkanoid.h"

int main(void){
	GAME_STATE estado_juego;

	if(!init(&estado_juego)){
		return -1;
	}

	estado_juego.state = menu();

	while(estado_juego.state == JUGAR){
		play(&estado_juego);
		if(estado_juego.state == DERROTA){
			end_phase(&estado_juego);
		}
	}

	de_init();

	return 0;
}



