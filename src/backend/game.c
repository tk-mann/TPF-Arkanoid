#include "game.h"
#include <stdio.h>
#include <math.h>

#define TIME_TICK (1.0f/60.0f)
#define GENERIC_W 512
#define GENERIC_H 512
#define PI 3.14159265f
enum CARA { LEFT, RIGHT, UP, DOWN };

void actualizar_bala(BALL *ball) {
    // Actualizar posición de la pelota
    //las posiciónes se actualizan en función de la velocidad
    ball->prev_x = ball->x;
    ball->prev_y = ball->y;
    ball->x += ball->vx;
    ball->y += ball->vy;

}

// Método mejorado de rebote basado en delta y aspect ratio
// Usamos el centro de la pelota

//NOTA MIA, LA VERDAD NO ENTENDÍ BIEN CÓMO FUNCIONA, lo saqué de una página
//pero como parece que funciona bien lo dejo
/*
                
Handling deflections off of blocks and walls is trivial if you 
keep track of your ball’s angle as a direction vector, and keep
 your blocks orthogonal. On a vertical hit, just invert the y 
 component of the direction vector, and on a horizontal hit, 
 invert the x component. One common question is how to tell 
 whether a hit is vertical or horizontal. You could inspect the
  collision details and look at surface normals, but with 
  orthogonal blocks, that’s a bit more complex than what is 
  needed. Just take the difference in position (delta) between
   the ball and block, scale it by the inverse aspect ratio of
    the block (for example, if the block is twice as long as it
     is high, divide the x component of the delta by 2), and see
      which component of the delta is smaller. A smaller y means
       a vertical hit, and a smaller x means a horizontal hit.
Here’s an example, with an additional Sign check added to make
 sure the ball is traveling toward the block. That check prevents
  the ball from destroying two adjacent blocks simultaneously.

var delta = (ball.transform.position - block.transform.position);
// apply aspect ratio via the scaleFactor vector
// For a horizontal block twice a wide as high, use Vector3(0.5f, 1f, 1f)
// For a vertical block twice as high as wide, use Vector3(1f, 0.5f, 1f)
// For a square block, use Vector3(1f, 1f, 1f), and so forth.
delta.Scale(scaleFactor); 
if(Mathf.Abs(delta.x) >= Mathf.Abs(delta.y))
{
    // scaled delta x was larger than delta y. This is a horizontal hit.
    if(Mathf.Sign(-direction.x) == Mathf.Sign(delta.x))
    {
        ballDirection.x = -ballDirection.x;
        ScoreBlockHit();
    }
}
else
{
    // scaled delta y was larger than delta x. This is a vertical hit.
    if(Mathf.Sign(-direction.y) == Mathf.Sign(delta.y))
    {
        ballDirection.y = -ballDirection.y;
        ScoreBlockHit();
    }
} 
                */

//si el golpe fue en el eje x, devuelve 1, 
int determinar_cara(BALL *ball, int x, int y, int width, int height) {
    // Resolver usando posición anterior para evitar que la pelota se meta en el objeto
    // Fallback: usar delta y aspect ratio
    float ball_center_x = ball->x + ball->size / 2.0f;
    float ball_center_y = ball->y + ball->size / 2.0f;
    
    // Centro del bloque
    float block_center_x = x + width / 2.0f;
    float block_center_y = y + height / 2.0f;
    
    // Delta desde el centro del bloque al centro de la pelota
    float delta_x = ball_center_x - block_center_x;
    float delta_y = ball_center_y - block_center_y;
    
    // Escalar delta por relación de aspecto inversa del bloque
    float aspect_ratio = (float)width / (float)height;
    delta_x = delta_x / aspect_ratio;
    
    // Determinar si es golpe horizontal o vertical comparando valores escalados
    if(fabs(delta_x) > fabs(delta_y))
    {
        // Golpe horizontal
        if(delta_x > 0) {
            ball->x = x + width;
            return RIGHT;
        } else {
            ball->x = x - ball->size;
            return LEFT;
        }
    }
    else
    {
        // Golpe vertical
        if(delta_y > 0) {
            ball->y = y + height;
            return DOWN;
        } else {
            ball->y = y - ball->size;
            return UP;
        }
    }
}

int detectar_colisiones(GAME_STATE *estado_juego, BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks) {
    //deteccion colisión pared izquierda
    if(ball->x <= 0){
        ball->x = 0;
        ball->vx = -ball->vx;
    }
    //deteccion colisión pared derecha
    else if(ball->x >= GENERIC_W - ball->size){
        ball->x = GENERIC_W- ball->size;
        ball->vx = -ball->vx;
    }
    //deteccion colisión techo
    if(ball->y <= 0){
        ball->y = 0;
        ball->vy = -ball->vy;
    }
    if(ball->y >= GENERIC_H - ball->size){
        ball->y = GENERIC_H - ball->size;
        ball->vy = -ball->vy;
    }
    //detección colisión bloque
    int i;
    for(i = 0; i < num_blocks; i++){
        if(blocks[i].alive){
            if(ball->x < blocks[i].x + blocks[i].width &&
            ball->x + ball->size > blocks[i].x &&
            ball->y < blocks[i].y + blocks[i].height &&
            ball->y + ball->size > blocks[i].y){ 
                int cara = determinar_cara(ball, blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].height);
                if(cara == LEFT || cara == RIGHT){
                    ball->vx = -ball->vx;
                } else {
                    ball->vy = -ball->vy;
                }
                if(blocks[i].type != UNBREAKABLE){
                    blocks[i].dureza--;
                    printf("Colisión con bloque, tipo: %d, dureza restante: %d\n", blocks[i].type, blocks[i].dureza);
                    if((blocks[i].dureza) <= 0){
                        if(blocks[i].type == HEAVY){
                            estado_juego->points += 20;
                        } else {
                            estado_juego->points += 10;
                        }
                    blocks[i].alive = false;
                    blocks[i].dead_block++;
                    }
                }
                return;
            }
        }
    }
    //detección colisión jugador
    if(ball->x < player->x + player->width &&
        ball->x + ball->size > player->x &&
        ball->y < player->y + player->height &&
        ball->y + ball->size > player->y){
        int cara = determinar_cara(ball, player->x, player->y, player->width, player->height);
        printf("Colisión con jugador, cara: %d\n", cara);
        if(cara == UP){
            // Ajustar la velocidad horizontal de la pelota según el punto de impacto en el jugador
            float impact_point = (ball->x + ball->size / 2.0f) - (player->x + player->width / 2.0f);
            float max_impact = player->width / 2.0f;
            float normalized_impact = impact_point / max_impact; // Rango de -1 a 1
            float speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy);
            float sign = (normalized_impact >= 0.0f) ? 1.0f : -1.0f;

            if(fabs(normalized_impact) <= 0.3f) {
                //printf("<0.3\n");
                ball->vy = -ball->vy;
            }
            else if(fabs(normalized_impact) > 0.3f && fabs(normalized_impact) <= 0.7f){
                //printf("<0.7\n");
                float angle_deg = 30.0f;
                float angle_rad = angle_deg * (PI / 180.0f);
                ball->vx = sign * speed * cosf(angle_rad);
                ball->vy = -speed * sinf(angle_rad);
            } else {
                printf(">0.7\n");
                float angle_deg = 150.0f;
                float angle_rad = angle_deg * (PI / 180.0f);
                ball->vx = -sign * speed * cosf(angle_rad);
                ball->vy = -speed * sinf(angle_rad);
            }
        }
    }
    return;
}


void detectar_condiciones(GAME_STATE* estado, BLOCK *blocks, BALL *ball, int num_blocks) {
    if(ball->y + ball->size >= GENERIC_H) {
        estado->vidas--;
        ball->state = RESET;
    }
    if(estado->vidas <= 0 || blocks->dead_block >= num_blocks) {
        estado->state = DERROTA; // Game Over
    }
    if(blocks->dead_block >= num_blocks) {
        estado->state = VICTORIA; // Victoria
    }
}