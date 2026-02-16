#include "frontend.h"
#include "game.h"
#include "game_struct.h"
#include <allegro5/allegro_color.h>

// Variables externas
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_EVENT event;
extern ALLEGRO_FONT * font;

#define BLOCK_WIDTH  (ALLEGRO_W / 11.0f)  // ≈ 46.5 píxeles
#define BLOCK_HEIGHT 16.0f 
#define LVL_1_BLOCKS 66 
#define PLAYER_START_X (ALLEGRO_W / 2 - 50) // Centrado horizontalmente
#define PLAYER_START_Y (ALLEGRO_H - 50) // Cerca del borde inferior
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 20

enum KEYS { KEY_LEFT=0, KEY_RIGHT, KEY_SPACE, KEY_ESCAPE };

//variables de posicion y movimiento
#define NAVE_MOVE_RATE  4   
#define BALL_MOVE_RATE 3

// Estado de teclas
static bool key_pressed[4] = {false, false, false, false};
static bool not_duplicate = true;

typedef struct{
	BLOCK block[LVL_1_BLOCKS];
    ALLEGRO_BITMAP *block_bitmaps[LVL_1_BLOCKS];
} BLOCK_ARRANGE_1;

typedef struct{
	ALLEGRO_BITMAP *player_bitmap;
    PLAYER player;
} ALL_PLAYER;

typedef struct{
	ALLEGRO_BITMAP *ball_bitmap;
    BALL ball;
} ALL_BALL;

void actualizar_movimientos(ALL_PLAYER *player, ALL_BALL *ball) {
    // Aquí puedes agregar lógica para actualizar la animación de los bloques, la pelota y el jugador
    // Por ejemplo, podrías cambiar el bitmap del bloque según su estado o animar la pelota al moverse
    al_draw_bitmap(ball->ball_bitmap, ball->ball.x, ball->ball.y, 0);
    al_draw_bitmap(player->player_bitmap, player->player.x, player->player.y, 0);
    //ACTUALIZAR BVLOQUJES FUNCOION
}

//YA TERMINE BÁSICAMENTE LA IMPRESIÓN EN PANBTALLA
//AHORA DEBERÍA PONERME A VER EL TEMA DEL BACKEND
//Y COMO SE MUEVE LA PELOTA E INTERACTUA CON EL ENTORNO
//QUE PASA CUANDO TETECTA COLISION Y TODO ESO


static void registrar_teclas() {
  if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
    switch (event.keyboard.keycode) {
    case ALLEGRO_KEY_LEFT:
      key_pressed[KEY_LEFT] = true;
      break;
    case ALLEGRO_KEY_RIGHT:
      key_pressed[KEY_RIGHT] = true;
      break;
    case ALLEGRO_KEY_SPACE:
      key_pressed[KEY_SPACE] = true;
      break;
    case ALLEGRO_KEY_ESCAPE:
      key_pressed[KEY_ESCAPE] = true;
      break;
    }
  } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
    switch (event.keyboard.keycode) {
    case ALLEGRO_KEY_LEFT:
      key_pressed[KEY_LEFT] = false;
      break;

    case ALLEGRO_KEY_RIGHT:
      key_pressed[KEY_RIGHT] = false;
      break;
    case ALLEGRO_KEY_SPACE:
      key_pressed[KEY_SPACE] = false;
      break;
    case ALLEGRO_KEY_ESCAPE:
      key_pressed[KEY_ESCAPE] = false;
      break;
    }
  }
}

void procesar_entradas(GAME_STATE *estado_juego, ALL_PLAYER * player, ALL_BALL * ball) {
    printf("posicion actual: %d, LEFT=%d, RIGHT=%d\n", player->player.x, key_pressed[KEY_LEFT], key_pressed[KEY_RIGHT]);
    if (key_pressed[KEY_LEFT]) {
          if ((player->player.x - NAVE_MOVE_RATE) >= 0) {
            player->player.x -= NAVE_MOVE_RATE;
          }
        }
        //procesa movimiento hacia lado derecho
        else if (key_pressed[KEY_RIGHT]) {
          if ((player->player.x + NAVE_MOVE_RATE) <= (ALLEGRO_W - PLAYER_WIDTH)) 
            player->player.x += NAVE_MOVE_RATE;
        }
        //procesa disparo
        if (key_pressed[KEY_SPACE] /*&& ball->ball.start == false*/) {
            ball->ball.start = true;
            ball->ball.y -= BALL_MOVE_RATE; // Iniciar movimiento de la pelota
        }
        //procesa pausa
        if (key_pressed[KEY_ESCAPE]) {
          estado_juego -> state = menu_pausa();
          key_pressed[KEY_ESCAPE] = 0;
        }
}


static ALLEGRO_COLOR get_color_from_enum(BlockColor color) {
    switch (color) {
        case COLOR_RED:    return al_map_rgb(255, 0, 0);
        case COLOR_ORANGE: return al_map_rgb(255, 165, 0);
        case COLOR_YELLOW: return al_map_rgb(255, 255, 0);
        case COLOR_GREEN:  return al_map_rgb(0, 255, 0);
        case COLOR_CYAN:   return al_map_rgb(0, 255, 255);
        case COLOR_BLUE:   return al_map_rgb(0, 0, 255);
        case COLOR_PURPLE: return al_map_rgb(128, 0, 128);
        case COLOR_PINK:   return al_map_rgb(255, 192, 203);
        case COLOR_WHITE:  return al_map_rgb(255, 255, 255);
        case COLOR_GRAY:   return al_map_rgb(128, 128, 128);
        default:           return al_map_rgb(255, 0, 0);
    }
}

ALLEGRO_BITMAP* create_ball_bitmap(float radius, ALLEGRO_COLOR color) {
    // Crear bitmap del tamaño necesario
    int size = (int)(radius * 2 + 2);
    ALLEGRO_BITMAP *ball = al_create_bitmap(size, size);
    
    // Guardar el target actual
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    
    // Dibujar EN el bitmap
    al_set_target_bitmap(ball);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));  // Transparente
    
    // Dibujar el círculo en el centro del bitmap
    al_draw_filled_circle(
        radius + 1,  // Centro X
        radius + 1,  // Centro Y
        radius,
        color
    );
    
    // Restaurar el target
    al_set_target_bitmap(old_target);
    
    return ball;
}

static ALLEGRO_BITMAP* create_block_bitmap(BlockColor color, int width, int height) {
    
    // Crear bitmap
    ALLEGRO_BITMAP *block = al_create_bitmap(width, height);
    if (!block) return NULL;
    
    // Guardar target actual
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    
    // Dibujar EN el bitmap
    al_set_target_bitmap(block);
    
    ALLEGRO_COLOR main_color = get_color_from_enum(color);
    
    // Fondo del bloque
    al_clear_to_color(main_color);
    
    
    // Borde más oscuro (efecto 3D)
    float r, g, b, a;
    al_unmap_rgba_f(main_color, &r, &g, &b, &a);

    ALLEGRO_COLOR dark = al_map_rgb_f(
        r * 0.6f,
        g * 0.6f,
        b * 0.6f
    );
    
    al_draw_rectangle(0, 0, width - 1, height - 1, dark, 1);
    
    // Highlight superior izquierdo (efecto 3D)
    ALLEGRO_COLOR light = al_map_rgb_f(
        (r * 1.3f > 1.0f ? 1.0f : r * 1.3f),
        (g * 1.3f > 1.0f ? 1.0f : g * 1.3f),
        (b * 1.3f > 1.0f ? 1.0f : b * 1.3f)
    );
    
    al_draw_line(1, 1, width - 2, 1, light, 1);  // Top
    al_draw_line(1, 1, 1, height - 2, light, 1); // Left
    
    // Restaurar target
    al_set_target_bitmap(old_target);
    
    return block;
}


int init_level_1(GAME_STATE *estado_juego, BLOCK_ARRANGE_1 *arrangement) {
    // Aquí puedes inicializar las variables del juego para el nivel 1
    // Por ejemplo, podrías cargar un mapa específico para el nivel 1
  // Inicializar bloques
  const int rows = 6;
  const int cols = 11;
  const float start_x = 0.0f;
  const float start_y = 30.0f;
  const float padding = 0.0f;

  BlockColor row_colors[6] = {
      COLOR_GRAY,
      COLOR_RED,
      COLOR_BLUE,
      COLOR_ORANGE,
      COLOR_PINK,
      COLOR_GREEN
  };

  int index = 0;
  for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {
          if (index >= LVL_1_BLOCKS) {
              break;
          }
          float x = start_x + c * (BLOCK_WIDTH + padding);
          float y = start_y + r * (BLOCK_HEIGHT + padding);

          arrangement->block[index].x = (int)x;
          arrangement->block[index].y = (int)y;
          arrangement->block[index].width = (int)BLOCK_WIDTH;
          arrangement->block[index].height = (int)BLOCK_HEIGHT;
          arrangement->block[index].type = 0;
          arrangement->block[index].alive = true;

          arrangement->block_bitmaps[index] = create_block_bitmap(row_colors[r], (int)BLOCK_WIDTH, (int)BLOCK_HEIGHT);
          al_draw_bitmap(arrangement->block_bitmaps[index], x, y, 0);
          index++;
      }
  }
  return 0;
}

int init_level_2(GAME_STATE *estado_juego) {
    // Aquí puedes inicializar las variables del juego para el nivel 2
    // Por ejemplo, podrías cargar un mapa específico para el nivel 2
}


void load_game(GAME_STATE *estado_juego, ALL_PLAYER *player, ALL_BALL *ball, BLOCK_ARRANGE_1 *arrangement) {
    // Aquí puedes inicializar las variables del juego según el nivel
    // Por ejemplo, podrías cargar un mapa específico para cada nivel
    al_clear_to_color(al_map_rgb(0, 0, 0));
    player->player_bitmap =  create_block_bitmap(COLOR_ORANGE, PLAYER_WIDTH, PLAYER_HEIGHT);
    al_draw_bitmap(player->player_bitmap, PLAYER_START_X, PLAYER_START_Y, 0);
    ball->ball_bitmap = create_ball_bitmap(8, al_map_rgb(255, 255, 255));
    al_draw_bitmap(ball->ball_bitmap, PLAYER_START_X + 50 - 8, PLAYER_START_Y - 16, 0);
    ball->ball.x = PLAYER_START_X + 50 - 8;
    ball->ball.y = PLAYER_START_Y - 16;
    player->player.x = PLAYER_START_X;
    player->player.y = PLAYER_START_Y;
    switch(estado_juego->level) {
        case 1:
            init_level_1(estado_juego, arrangement);
             break;
        case 2:
            init_level_2(estado_juego);
            break;
        // Agrega más casos para niveles adicionales
        default:
            // Inicializar un nivel por defecto o mostrar un mensaje de error
            break;
    }
   al_flip_display();
}



void play(GAME_STATE *estado_juego) {
    ALL_PLAYER player;
    ALL_BALL ball;
    BLOCK_ARRANGE_1 arrangement;
    ALLEGRO_TIMER *timer = NULL;
    printf("aca bien\n");
    load_game(estado_juego, &player, &ball, &arrangement);
    //printf("juego cargado\n");
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        estado_juego -> state = SALIR;
    }
    al_register_event_source(event_queue, al_get_keyboard_event_source()); //REGISTRAMOS EL TECLAD
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    //printf("timer iniciado\n");
    while(estado_juego->state == JUGAR) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        //printf("dentro del while\n");
        al_wait_for_event(event_queue, &event);
        
        if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
            registrar_teclas();
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            procesar_entradas(estado_juego, &player, &ball);
            actualizar_movimientos(&player, &ball);
            al_flip_display();
        }
        //printf("procesar entradas\n");
    }







        // Limpiar recursos
    al_destroy_timer(timer);
    al_destroy_bitmap(player.player_bitmap);
    al_destroy_bitmap(ball.ball_bitmap);
    for (int i = 0; i < LVL_1_BLOCKS; i++) {
        if (arrangement.block_bitmaps[i]) {
            al_destroy_bitmap(arrangement.block_bitmaps[i]);    
        }
    }
}