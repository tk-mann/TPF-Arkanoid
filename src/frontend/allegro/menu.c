#include "frontend.h"

// Variables externas
extern ALLEGRO_DISPLAY * display;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_EVENT event;
extern ALLEGRO_FONT * font;

static void get_button_state(bool * button_state, ALLEGRO_BITMAP **imagen, ALLEGRO_BITMAP ** backsquare_1, ALLEGRO_BITMAP ** backsquare_2) {
   if (event.mouse.x >= 160 && event.mouse.x < 460 && event.mouse.y >= 100 && event.mouse.y < 136 && !button_state[0]) {
      al_draw_tinted_bitmap(*backsquare_1, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 65, 90, 0);
      al_flip_display();
      button_state[0] = true;
   } else if (event.mouse.x >= 160 && event.mouse.x < 460 &&
      event.mouse.y >= 300 && event.mouse.y < 336 && !button_state[1]) {
      al_draw_tinted_bitmap(*backsquare_2, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 30, 290, 0);
      al_flip_display();
      button_state[1] = true;
   }

   if (!(event.mouse.x >= 160 && event.mouse.x < 460 && event.mouse.y >= 100 && event.mouse.y < 136) && !(event.mouse.x >= 160 && event.mouse.x < 460 &&
         event.mouse.y >= 300 && event.mouse.y < 336)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_bitmap(*imagen, 0, 0, 0);
      al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 100, ALLEGRO_ALIGN_CENTER, "Iniciar Juego");
      al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 300, ALLEGRO_ALIGN_CENTER, "Salir");
      al_flip_display();
      button_state[0] = false;
      button_state[1] = false;
   }
}

static int iniciar_texto(ALLEGRO_BITMAP **imagen, ALLEGRO_BITMAP ** backsquare_1, ALLEGRO_BITMAP ** backsquare_2) {

	//cargar imagen de fondo
   *imagen = al_load_bitmap("assets/nave/fondo_inicio.png");

   // verificar que se cargó correctamente
   if (!*imagen) {
      return 1;
   }

   //crear y verificar bitmaps
   *backsquare_1 = al_create_bitmap(130, 36);
   if (!*backsquare_1) {
      return 1;
   }
   *backsquare_2 = al_create_bitmap(60, 36);
   if (!*backsquare_2) {
      return 1;
   }

   al_set_target_bitmap(*backsquare_1); //Setea el bitmap a dibujar
   al_clear_to_color(al_map_rgb(100, 100, 100)); //Rellena el bitmap
   al_set_target_bitmap(al_get_backbuffer(display)); //Setea el bitmap  en el display

   al_set_target_bitmap(*backsquare_2);
   al_clear_to_color(al_map_rgb(100, 100, 100));
   al_set_target_bitmap(al_get_backbuffer(display));

   //dibujar el texto
   al_clear_to_color(al_map_rgb(0, 0, 0));
   al_draw_bitmap(*imagen, 0, 0, 0);
   al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 100, ALLEGRO_ALIGN_CENTER, "Iniciar Juego");
   al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 300, ALLEGRO_ALIGN_CENTER, "Salir");

   al_flip_display();
   return 0;
}

int menu() {
	//iniciar eventos y variables
   al_get_next_event(event_queue, & event);
   bool salir = false;
   bool button_state[2] = {false, false}; //el primner elemento representa iniciiar y segundo salir
   int opcion;

   //inicializar texos y bitmaps
   ALLEGRO_BITMAP *imagen = NULL;
   ALLEGRO_BITMAP *backsquare_1 = NULL;
   ALLEGRO_BITMAP *backsquare_2 = NULL;


   if (iniciar_texto(&imagen, &backsquare_1, &backsquare_2)) {
      opcion = SALIR;
      salir = true;
   }

   while (!salir) {
      al_get_next_event(event_queue, & event);

      //dibujar mientras se esté moviendo mouse
      if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
         get_button_state(button_state, &imagen, &backsquare_1, &backsquare_2);
      }

      //detectar motones presionados
      if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
         if (button_state[0]) {
            opcion = JUGAR;
            salir = true;
         } else if (button_state[1]) {
            opcion = SALIR;
            salir = true;
         }
      }

      //cerrar display
      if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         salir = true;
         opcion = SALIR;
      }
   }

   //borrar bitfields
   al_destroy_bitmap(backsquare_1);
   al_destroy_bitmap(backsquare_2);
   al_destroy_bitmap(imagen);
   return opcion;
}

int menu_pausa() {
	//crear bitmaps
   ALLEGRO_BITMAP * backsquare_1 = al_create_bitmap(100, 50);
   ALLEGRO_BITMAP * backsquare_2 = al_create_bitmap(100, 50);
   ALLEGRO_BITMAP * backsquare_3 = al_create_bitmap(100, 50);
   ALLEGRO_BITMAP *imagen;
   imagen = al_load_bitmap("assets/nave/fondo_inicio.png");

   // Rellenar los bitmaps con color blanco
   al_set_target_bitmap(backsquare_1);
   al_clear_to_color(al_map_rgb(255, 255, 255));
   al_set_target_bitmap(backsquare_2);
   al_clear_to_color(al_map_rgb(255, 255, 255));
   al_set_target_bitmap(backsquare_3);
   al_clear_to_color(al_map_rgb(255, 255, 255));
   al_set_target_bitmap(al_get_backbuffer(display));

   al_draw_bitmap(imagen, 0, 0, 0);

   bool salir_menu = false;
   int opcion = 0;
   bool redraw = true;

   while (!salir_menu) {
      al_get_next_event(event_queue, & event);

      if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         salir_menu = true;
         opcion = SALIR;
      }

      if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
         redraw = true; // Redibujar cuando se mueve el mouse
      }

      if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
         int mx = event.mouse.x;
         int my = event.mouse.y;

         // Botón Reanudar
         if (mx >= 110 && mx <= 310 && my >= 150 && my <= 200) {
            salir_menu = true;
            opcion = JUGAR;
         }
         // Botón Reiniciar
         else if (mx >= 110 && mx <= 310 && my >= 250 && my <= 300) {
            salir_menu = true;
            opcion = REINICIAR;
         }
         // Botón Salir
         else if (mx >= 110 && mx <= 310 && my >= 350 && my <= 400) {
            salir_menu = true;
            opcion = SALIR;
         }
      }

      if (redraw) {
         redraw = false;

         // Obtener posición actual del mouse
         ALLEGRO_MOUSE_STATE state;
         al_get_mouse_state( & state);
         int mx = state.x;
         int my = state.y;

         // Limpiar pantalla
         al_clear_to_color(al_map_rgb(0, 0, 0));

         al_draw_bitmap(imagen, 0, 0, 0);

         // Dibujar resaltado si el mouse está sobre un botón
         if (mx >= 110 && mx <= 310 && my >= 150 && my <= 200) {
            al_draw_tinted_bitmap(backsquare_1, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 50, 150, 0);
         } else if (mx >= 110 && mx <= 310 && my >= 250 && my <= 300) {
            al_draw_tinted_bitmap(backsquare_2, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 50, 250, 0);
         } else if (mx >= 110 && mx <= 310 && my >= 350 && my <= 400) {
            al_draw_tinted_bitmap(backsquare_3, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 50, 350, 0);
         }

         // Dibujar texto encima
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 100, ALLEGRO_ALIGN_CENTER, "PAUSA");
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 165, ALLEGRO_ALIGN_CENTER, "Reanudar");
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 265, ALLEGRO_ALIGN_CENTER, "Reiniciar");
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 365, ALLEGRO_ALIGN_CENTER, "Salir");

         al_flip_display();
      }
   }

   al_destroy_bitmap(backsquare_1);
   al_destroy_bitmap(backsquare_2);
   al_destroy_bitmap(backsquare_3);
   al_destroy_bitmap(imagen);

   return opcion;
}

void end_phase(GAME_STATE * estado_juego) {
   ALLEGRO_BITMAP * backsquare_1 = al_create_bitmap(150, 50);
   ALLEGRO_BITMAP * backsquare_2 = al_create_bitmap(60, 50);
   ALLEGRO_BITMAP *imagen;
   imagen = al_load_bitmap("assets/nave/fondo_inicio.png");

   // Rellenar los bitmaps con color blanco
   al_set_target_bitmap(backsquare_1);
   al_clear_to_color(al_map_rgb(255, 255, 255));
   al_set_target_bitmap(backsquare_2);
   al_clear_to_color(al_map_rgb(255, 255, 255));
   al_set_target_bitmap(al_get_backbuffer(display));

   al_draw_bitmap(imagen, 0, 0, 0);

   bool salir_menu = false;
   bool redraw = true;

   while (!salir_menu) {
      al_get_next_event(event_queue, & event);

      if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         salir_menu = true;
         estado_juego -> state = SALIR;
      }

      if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
         redraw = true; // Redibujar cuando se mueve el mouse
      }

      if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
         int mx = event.mouse.x;
         int my = event.mouse.y;

         // Botón Jugar de Nuevo (110, 300)
         if (mx >= 110 && mx <= 310 && my >= 300 && my <= 350) {
            salir_menu = true;
            estado_juego -> state = JUGAR;
            estado_juego -> vidas = 3;
            estado_juego -> points = 0;
            estado_juego -> level = 1;
         }
         // Botón Salir (110, 400)
         else if (mx >= 110 && mx <= 310 && my >= 400 && my <= 450) {
            salir_menu = true;
            estado_juego -> state = SALIR;
         }
      }

      if (redraw) {
         redraw = false;

         // Obtener posición actual del mouse
         ALLEGRO_MOUSE_STATE state;
         al_get_mouse_state( & state);
         int mx = state.x;
         int my = state.y;

         // Limpiar pantalla
         al_clear_to_color(al_map_rgb(0, 0, 0));

         al_draw_bitmap(imagen, 0, 0, 0);

         // Dibujar resaltado si el mouse está sobre un botón
         if (mx >= 110 && mx <= 310 && my >= 300 && my <= 350) {
            al_draw_tinted_bitmap(backsquare_1, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 75, 300, 0);
         } else if (mx >= 110 && mx <= 310 && my >= 400 && my <= 450) {
            al_draw_tinted_bitmap(backsquare_2, al_map_rgba(100, 100, 100, 100), ALLEGRO_W / 2 - 30, 400, 0);
         }

         // Textos
         al_draw_text(font, al_map_rgb(255, 0, 0), ALLEGRO_W / 2, 150, ALLEGRO_ALIGN_CENTER, "HAS PERDIDO");

         char texto_puntaje[50];
         sprintf(texto_puntaje, "Puntaje: %d", estado_juego -> points);
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 220, ALLEGRO_ALIGN_CENTER, texto_puntaje);

         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 315, ALLEGRO_ALIGN_CENTER, "Jugar de Nuevo");
         al_draw_text(font, al_map_rgb(255, 255, 255), ALLEGRO_W / 2, 415, ALLEGRO_ALIGN_CENTER, "Salir");

         al_flip_display();
      }
   }

   al_destroy_bitmap(backsquare_1);
   al_destroy_bitmap(backsquare_2);
   al_destroy_bitmap(imagen);
}
