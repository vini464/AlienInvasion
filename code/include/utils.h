#ifndef UTILS_H
#define UTILS_H

typedef struct {
  int x;
  int y;
} pair;

typedef struct {
  pair position;
  int  asset_id;
  int  reg_id;
  int height;
  int width;
} player;

#define MAX_SCREEN_WIDTH   640
#define MAX_SCREEN_HEIGHT  480


#endif // !UTILS_H

