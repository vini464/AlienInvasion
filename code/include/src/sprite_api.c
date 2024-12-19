#include "../sprite_api.h"

Sprite *createSprite(int initial_posx, int initial_posy, int act, int mem_offset, int reg_id) {
  Sprite *new_sprite = (Sprite *)malloc(sizeof(Sprite));
  new_sprite->pos_x = initial_posx;
  new_sprite->pos_y = initial_posy;
  new_sprite->act = act;
  new_sprite->mem_offset = mem_offset;
  new_sprite->reg_id = reg_id;
  return new_sprite;
}

int checkCollision(Sprite *sp1, Sprite *sp2) {
  int length_x = sp1->pos_x - sp2->pos_x;
  int length_y = sp1->pos_y - sp2->pos_y;

  if (length_x < 0)
    length_x *= -1;
  if (length_y < 0)
    length_y *= -1;

  return length_x < 20 && length_y < 20 ? 1 : 0;
}

void showSprite(Sprite *sp) {
  wbr_sp(sp->act, sp->pos_x, sp->pos_y, sp->mem_offset, sp->reg_id);
}

void updateSpritePosition(Sprite *sp, int pos_x, int pos_y) {
  sp->pos_x = pos_x;
  sp->pos_y = pos_y;
}

void deleteSprite(Sprite *sp) { 
  free(sp);
}
