#include "collision.h"
#include "utils.h"
#include <stdlib.h>

int check_collide(player p1, player p2){
  pair point1, point2;
  point1.x = p1.position.x + p1.width/2;
  point1.y = p1.position.y + p1.height/2;

  point2.x = p2.position.x + p2.width/2;
  point2.y = p2.position.y + p2.height/2;

  return abs(point1.x - point2.x) < (p1.width + p2.width)/2 && abs(point1.y - point2.y) < (p1.width + p2.width)/2? 1 : 0;

}
