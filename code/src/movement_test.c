#include "../include/gpu.h"
#include "../include/accel.h"
#include "../include/utils.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#define EVENTPATH "/dev/input/event0"
#define MAX_SHOTS 3
// contém todas as informações de um jogador
// para usar na fução wbr
typedef struct {
  pair position;
  int asset_id;
  int reg_id;
  int speed;
} shot;

typedef struct {
  pair position;
  int  asset_id;
  int  reg_id;
  int  active_shots;
  shot shoots[MAX_SHOTS];
} player;

typedef struct {
  struct timeval time;
  unsigned short type;
  unsigned short code;
  unsigned int value;
} input_event;


// Globals;
int FD, LISTEN_ACCEL, LISTEN_MOUSE, SHOW_IMAGES, LISTEN_BUTTONS, buttomPressed, SHOOTING;
pair ACCEL_DEGS;
player P1, P2;
shot tiro;

void *mouseListener(void *);  // Roda em uma nova thread lendo o mouse
void *update_shot(void *);
void *buttomListener(void *); // Roda em uma nova thread lendo os botoes
void *accelListener(void *);  // Roda em uma nova thread lendo o acelerômetro
void startAccelListener();    // inicia a comunicação com o acelerômetro
void stopAccelListener();     // encerra acomunicação com o acelerômetro
void *render(void *);         // thread para redenizar os sprites na tela
void player_shot(player *player, shot *tiro); // faz o personagem atirar

int main(void){
  pthread_t mouse_t, accel_t, render_t, buttom_t, updateShot_t;
  char stop;
  
  P1.reg_id = 1;
  P1.asset_id = 1;
  P1.position.x = 310;
  P1.position.y = 430;

  P2.reg_id = 2;
  P2.asset_id = 7;
  P2.position.x = 310;
  P2.position.y = 430;

  tiro.position.x = 0;
  tiro.position.y = 0;
  tiro.asset_id = 4;
  tiro.reg_id = 5;
  tiro.speed = 3;
  SHOW_IMAGES = 1;

  gpu_open();

  LISTEN_ACCEL = 1;
  LISTEN_MOUSE = 1;
  LISTEN_BUTTONS = 1;
  SHOOTING = 0;
  startAccelListener(); // inicia a comunicação com o acelerômetro

  wbr_sp(1, P1.position.x, P1.position.y, P1.asset_id, P1.reg_id); // faz o primeiro desenho do personagem
  
  pthread_create(&accel_t, NULL, accelListener, NULL); // cria a thread do acelerometro
  pthread_create(&render_t, NULL, render, NULL); // cria a thread do acelerometro
  pthread_create(&mouse_t, NULL, mouseListener, NULL); // cria a thread do acelerometro
  pthread_create(&buttom_t, NULL, buttomListener, NULL); // cria a thread do acelerometro
  pthread_create(&updateShot_t, NULL, update_shot, NULL); // cria a thread do tiro 
  
  // as threads rodam mesmo com o código aguardando isso aqui
  printf("Press enter to stop\n");
  scanf("%c", &stop);
  SHOW_IMAGES = 0;
  LISTEN_ACCEL = 0;
  LISTEN_MOUSE = 0;
  LISTEN_BUTTONS = 0;

  pthread_join(accel_t, NULL);
  pthread_join(render_t, NULL);
  pthread_join(mouse_t, NULL);
  pthread_join(buttom_t, NULL);
  pthread_join(updateShot_t, NULL);

  stopAccelListener();

  gpu_close();
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
    /*P1.position.y -= ACCEL_DEGS.y/10;*/

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
    int i;
    for (i = 0; i < P1.active_shots; i++){
      wbr_sp(1, P1.shoots[i].position.x, P1.shoots[i].position.y, P1.shoots[i].asset_id, P1.shoots[i].reg_id);
    }
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
      // isso aqui vai ter que mudar pra um lugar separado
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

void *buttomListener(void *arg){
  while(LISTEN_BUTTONS){
    buttomPressed = read_keys(); 
    if (buttomPressed == 14 && SHOOTING == 0){
      SHOOTING = 1;
      P1.active_shots++;
      SHOOTING = 0;
    }
  }
};

void creat_shot(int index){
  P1.shoots[index].position.x = P1.position.x;
  P1.shoots[index].position.y = P1.position.y - 20;
  P1.shoots[index].asset_id = 4; // tem que mudar aqui
  P1.shoots[index].reg_id = 5;
  P1.shoots[index].speed = 3;
}

void *update_shot(void *){
  while (1) {
    clock_t before, after;
    int msec = 0;

    int i;
    for (i = 0; i < P1.active_shots; i++){
      creat_shot(i);
    }
    while(1){
      for (i = 0; i < P1.active_shots; i++){
        do {
          clock_t difference = clock() -before;
          msec = difference * 1000 / CLOCKS_PER_SEC;
        } while (msec < 1);

        P1.shoots[i].position.y -= 2;
        if(P1.shoots[i].position.y >= 0){
          P1.active_shots--;
        }
      }
      break;
    }
  }
}

void player_shot(player *player, shot *tiro){
  // o sprite do tiro vai aumentar/diminuir a posição y e manter a posição x de quem desparou
  tiro->asset_id = 11;
  tiro->position.x = player->position.x;
  clock_t before, after;
  int msec = 0;
  tiro->position.y = player->position.y - 20;

  while(tiro->position.y >= 0){
    /*printf("%d\n", tiro->position.y);*/
    msec = 0;
    before = clock();
    do {
      clock_t difference = clock() -before;
      msec = difference * 1000 / CLOCKS_PER_SEC;
    } while (msec < 1);
    tiro->position.y -=2;
  }
  /*printf("parou de atirar\n");*/
  // vai parar/sumir quando atingir a borda ou outro pixel
  wbr_sp(0, tiro->position.x, tiro->position.y, tiro->asset_id, tiro->reg_id);
};
