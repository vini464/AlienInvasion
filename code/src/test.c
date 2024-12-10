#include "../include/accel.h"
#include "../include/mouse.h"
#include "../include/sprite_api.h"
#include "../include/utils.h"

#include <pthread.h>
#include <stdio.h>


typedef struct {
  Sprite * sp;
  int shoots;
  int life;
} Player;

typedef struct {
  Sprite *sp;
  int speed; 
} Shoot;



int main(void) {
  
  return 0; }
