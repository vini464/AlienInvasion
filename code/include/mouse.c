#include "mouse.h"
#include "utils.h"
#include <fcntl.h>


int open_mouse() {
  return open(EVENTPATH, O_RDONLY);
}
void close_mouse(int fd) {
  close(fd);
}

void read_mouse(int fd, mouse_event *mouse) {
  input_event ev;
  read(fd, &ev, sizeof(ev)); // lê todos os eventos;

  if (ev.type == 2) {
    if (ev.code == 0) {
      mouse->direction.x = ev.value;
      return;
    }

    if (ev.code == 1) {
      mouse->direction.y = ev.value;
      return;
    }

    if (ev.code == 8) { // rodinha do mouse
      mouse->scroll = ev.value;
      return;
    }
  }
  if (ev.type == 1) {

    if (ev.code == 272) {
      if (ev.value)
        mouse->btn_left = 1; // clicked
      return;
    }
    if (ev.code == 273) {
      if (ev.value)
        mouse->btn_right = 1; // clicked
      return;
    }
    if (ev.code == 274) {
      if (ev.value)
        mouse->btn_middle = 1; // clicked
      return;
    }
  }
}
