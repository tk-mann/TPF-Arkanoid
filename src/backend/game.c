#include "game.h"
#include <stdio.h>
#include <math.h>
#define TIME_TICK (1.0f/60.0f)
#define GENERIC_W 512
#define GENERIC_H 512

void actualizar_bala(BALL *ball) {
    // Actualizar posición de la pelota
    //las posiciónes se actualizan en función de la velocidad
    ball->prev_x = ball->x;
    ball->prev_y = ball->y;
    ball->x += ball->vx;
    ball->y += ball->vy;

}

int detectar_colisiones(BALL *ball, PLAYER *player, BLOCK *blocks, int num_blocks) {
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
    for(i = 0; i < 6*5; i++){
        if(blocks[i].alive){
            if(ball->x < blocks[i].x + blocks[i].width &&
               ball->x + ball->size > blocks[i].x &&
               ball->y < blocks[i].y + blocks[i].height &&
               ball->y + ball->size > blocks[i].y){ 
                blocks[i].alive = false;
                
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
                
                float ball_center_x = ball->x + ball->size / 2.0f;
                float ball_center_y = ball->y + ball->size / 2.0f;
                
                // Centro del bloque
                float block_center_x = blocks[i].x + blocks[i].width / 2.0f;
                float block_center_y = blocks[i].y + blocks[i].height / 2.0f;
                
                // Delta desde el centro del bloque al centro de la pelota
                float delta_x = ball_center_x - block_center_x;
                float delta_y = ball_center_y - block_center_y;
                
                // Escalar delta por relación de aspecto inversa del bloque
                float aspect_ratio = (float)blocks[i].width / (float)blocks[i].height;
                delta_x = delta_x / aspect_ratio;
                
                // Determinar si es golpe horizontal o vertical comparando valores escalados
                if(fabs(delta_x) > fabs(delta_y))
                {
                    // Golpe horizontal
                    ball->vx = -ball->vx;
                }
                else
                {
                    // Golpe vertical
                    ball->vy = -ball->vy;
                }
                return 0;
            }
        }
    }

}