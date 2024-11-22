#include "../include/gpu.h"
#include "../include/accel.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#define EVENTPATH "/dev/input/event0"

// contém todas as informações de um jogador
// para usar na fução wbr
typedef struct {
  pair position;
  int  asset_id;
  int  reg_id;
} player;

typedef struct {
  struct timeval time;
  unsigned short type;
  unsigned short code;
  unsigned int value;
}input_event;


// Globals;
int FD, LISTEN_ACCEL, LISTEN_MOUSE, SHOW_IMAGES;
pair ACCEL_DEGS;
player P1, P2;

void *mouseListener(void *);  // Roda em uma nova thread lendo o mouse
void *accelListener(void *);  // Roda em uma nova thread lendo o acelerômetro
void startAccelListener();    // inicia a comunicação com o acelerômetro
void stopAccelListener();     // encerra acomunicação com o acelerômetro
void *render(void *);


int main(void){
  pthread_t mouse_t, accel_t, render_t;
  char stop;
  P1.reg_id = 1;
  P1.asset_id = 1;
  P1.position.x = 310;
  P1.position.y = 430;
  P2.reg_id = 2;
  P2.asset_id = 7;
  P2.position.x = 310;
  P2.position.y = 430;

  SHOW_IMAGES = 1;
  gpu_open();
  LISTEN_ACCEL = 1;
  LISTEN_MOUSE = 1;
  startAccelListener(); // inicia a comunicação com o acelerômetro
  wbr_sp(1, P1.position.x, P1.position.y, P1.asset_id, P1.reg_id); // faz o primeiro desenho do personagem
  pthread_create(&accel_t, NULL, accelListener, NULL); // cria a thread do acelerometro
  pthread_create(&render_t, NULL, render, NULL); // cria a thread do acelerometro
  pthread_create(&mouse_t, NULL, mouseListener, NULL); // cria a thread do acelerometro
  printf("Press enter to stop\n");
  scanf("%c", &stop);
  SHOW_IMAGES = 0;
  LISTEN_ACCEL = 0;
  LISTEN_MOUSE = 0;
  pthread_join(accel_t, NULL);
  pthread_join(render_t, NULL);
  pthread_join(mouse_t, NULL);
  stopAccelListener();
  return 0;
}

void startAccelListener() {
  int i;
  for (i = 0; i < 10; i++) {
    FD = open_and_mmap_dev_mem();
    if (FD == -1)
      printf("não foi possível abrir /dev/mem\n");
    else
      break;
  }
  if (FD == -1)
    exit(-1);
  I2C0_init();
  accel_init();
  accel_calibrate(100);
}
void *accelListener(void *arg) {
  int accel_read = 0;
  pair direction;
  ACCEL_DEGS.x = 0;
  ACCEL_DEGS.y = 0;
  while (LISTEN_ACCEL) {
    get_direction(&direction);
  
    ACCEL_DEGS.x = direction.x;
    ACCEL_DEGS.y = direction.y;
    
//    printf("degs: %d %d\n", ACCEL_DEGS.x, ACCEL_DEGS.y);
    
    direction.x = 0;
    direction.y = 0;
      P1.position.x += ACCEL_DEGS.x/10;
      P1.position.y -= ACCEL_DEGS.y/10;
      
      if (P1.position.x < 0) 
        P1.position.x = 0;
      if (P1.position.x > 620) 
        P1.position.x = 620;
      if (P1.position.y < 0) 
        P1.position.y = 0;
      if (P1.position.y > 440) 
        P1.position.y = 440;

  usleep(10000);
  }
  return NULL;
}
void stopAccelListener() { close_and_unmap_dev_mem(FD); }

void *render(void *arg){
  while (SHOW_IMAGES){
      wbr_sp(1, P1.position.x, P1.position.y, P1.asset_id, P1.reg_id); 
      wbr_sp(1, P2.position.x, P2.position.y, P2.asset_id, P2.reg_id); 
  }
return NULL;
}

void *mouseListener(void *arg) {
  int mouse_fd = open(EVENTPATH, O_RDONLY);
  if (mouse_fd == -1)
    exit(1);
  input_event ev;
  while (LISTEN_MOUSE){
    read(mouse_fd, &ev, sizeof(ev));    
//    printf("type: %d, code: %d, val: %d\n", ev.type, ev.code, ev.value);
    if (ev.type == 2 && (ev.code == 0 || ev.code == 1)){
      if (ev.code == 0)
        P2.position.x += ev.value;
      else
        P2.position.y += ev.value;
      
      if (P2.position.x < 0) 
        P2.position.x = 0;
      if (P2.position.x > 620) 
        P2.position.x = 620;
      if (P2.position.y < 0) 
        P2.position.y = 0;
      if (P2.position.y > 440) 
        P2.position.y = 440;

        }
  }
 return NULL;
}



