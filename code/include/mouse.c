#include "mouse.h"
#include "utils.h"

void read_mouse(int fd, mouse_event *mouse) {
  input_event ev;
  read(fd, &ev, sizeof(ev)); // lê todos os eventos;

  if (ev.type == 2) {
    if (ev.code == 0) {
      mouse->position.x += ev.value;
      if (mouse->position.x > MAX_SCREEN_WIDTH)
        mouse->position.x = MAX_SCREEN_WIDTH;
      if (mouse->position.x < 0)
        mouse->position.x = 0;
      return;
    }

    if (ev.code == 1) {
      mouse->position.y += ev.value;
      if (mouse->position.y > MAX_SCREEN_HEIGHT)
        mouse->position.y = MAX_SCREEN_HEIGHT;
      if (mouse->position.y < 0)
        mouse->position.y = 0;
      return;
    }

    if (ev.code == 8) { // rodinha do mouse
      mouse->scroll = ev.value;
      return;
    }
    if (ev.code == 272) {
      if (ev.value || ev.time.tv_sec == mouse->last_event.tv_sec)
        mouse->btn_left = 1; // clicked
      return;
    }
    if (ev.code == 273) {
      if (ev.value || ev.time.tv_sec == mouse->last_event.tv_sec)
        mouse->btn_middle = 1; // clicked
      return;
    }
    if (ev.code == 274) {
      if (ev.value || ev.time.tv_sec == mouse->last_event.tv_sec)
        mouse->btn_right = 1; // clicked
      return;
    }
  }
}
