#ifndef MOUSE_H
#define MOUSE_H

#include "utils.h"
#include <sys/time.h>
#include <unistd.h>

#define EVENTPATH "/dev/input/event0"


// struct de leitura da informação do mouse
typedef struct {
  struct timeval time;
  unsigned short type;
  unsigned short code;
  unsigned int   value;
} input_event;

typedef struct {
  int btn_left;
  int btn_middle;
  int btn_right;
  pair direction;
  int scroll; // talvez remover
} mouse_event;

int open_mouse();
void read_mouse(int fd, mouse_event * mouse);
void close_mouse(int fd);

#endif // !MOUSE_H
