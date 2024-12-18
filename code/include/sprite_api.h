#ifndef SPRITE_API
#define SPRITE_API

#include "gpu.h"
#include <stdlib.h>

typedef struct {
  int pos_x, pos_y;
  int act, mem_offset, reg_id;
} Sprite;

Sprite * createSprite(int initial_posx, int initial_posy, int act, int mem_offset, int reg_id); // cria um novo sprite
int checkCollision(Sprite *sp1, Sprite *sp2); // checa a colisão entre dois sprites
void showSprite(Sprite *sp); // mostra o sprite na tela
void updateSpritePosition(Sprite *sp, int pos_x, int pos_y);
void deleteSprite(Sprite *sp);

#endif // !GPU_API
