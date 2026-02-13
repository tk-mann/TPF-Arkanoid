#include "frontend.h"

ALLEGRO_DISPLAY * display;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_EVENT event;
ALLEGRO_FONT * font;

/**
 * inicializar la librería y el proyecto
 */
int init(GAME_STATE *estado_juego) {

	estado_juego->level = 1;
	estado_juego->vidas = 3;
	estado_juego->points = 0;

	//inicializa allegro
   if (!al_init()) {
      return 0;
   }

   // 1. Inicializar addons
   if (!al_init_primitives_addon()) {
      return 0;
   }
   if (!al_init_image_addon()) {
      return 0;
   }
   al_init_font_addon();
   al_init_ttf_addon();

   // 2. Instalar dispositivos de entrada
   if (!al_install_keyboard()) {
      return 0;
   }
   if (!al_install_mouse()) {
      return 0;
   }

   // 3. Crear display PRIMERO
   display = al_create_display(ALLEGRO_W, ALLEGRO_H);
   if (!display) {
      return 0;
   }

   // 4. Cargar recursos
   font = al_load_ttf_font("res/disney.ttf", 16, 0);
   if (!font) {
      return 0;
   }

   // 5. Crear event queue DESPUÉS del display
   event_queue = al_create_event_queue();
   if (!event_queue) {
      al_destroy_display(display);
      return 0;
   }

   // 6. Registrar fuentes de eventos
   al_register_event_source(event_queue, al_get_display_event_source(display));
   al_register_event_source(event_queue, al_get_keyboard_event_source());
   al_register_event_source(event_queue, al_get_mouse_event_source());

   al_flip_display();
   return 1;
}


/**
 * borra objetos creados y libera memoria reservada
 */
void de_init(void) {
   // 1. Destruir event queue
   if (event_queue) {
      al_destroy_event_queue(event_queue);
      event_queue = NULL;
   }

   // 2. Destruir fuente
   if (font) {
      al_destroy_font(font);
      font = NULL;
   }

   // 3. Destruir display
   if (display) {
      al_destroy_display(display);
      display = NULL;
   }

   // 4. Desinstalar addons y dispositivos (en orden inverso a la inicialización)
   al_uninstall_mouse();
   al_uninstall_keyboard();

   // 5. Shutdown de addons
   al_shutdown_ttf_addon();
   al_shutdown_font_addon();
   al_shutdown_image_addon();
   al_shutdown_primitives_addon();
}
