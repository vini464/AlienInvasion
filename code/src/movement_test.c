#include "../include/gpu.h"
#include "../include/accel.h"
#include <pthread.h>

// contém todas as informações de um jogador
// para usar na fução wbr
typedef struct {
  pair position;
  int  asset_id;
  int  reg_id;
} player;

// Globals;
int FD, LISTEN_ACCEL, LISTEN_MOUSE;
player P1, P2;

void *mouseListener(void *);  // Roda em uma nova thread lendo o mouse
void *accelListener(void *);  // Roda em uma nova thread lendo o acelerômetro
void startAccelListener();    // inicia a comunicação com o acelerômetro
void stopAccelListener();     // encerra acomunicação com o acelerômetro


int main(void){
  pthread_t mouse_t, accel_t;
  char stop;
  P1.reg_id = 1;
  P1.asset_id = 1;
  P1.position.x = 310;
  P1.position.y = 430;

  startAccelListener(); // inicia a comunicação com o acelerômetro
  wbr_sp(1, P1.position.x, P1.position.y, P1.asset_id, P1.reg_id); // faz o primeiro desenho do personagem
  pthread_create(&accel_t, NULL, accelListener, NULL); // cria a thread do acelerometro
  printf("Press enter to stop\n");
  scanf("%c", &stop);
  LISTEN_ACCEL = 0;
  pthread_join(accel_t, NULL);
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
  accel_calibrate(60);
}
void *accelListener(void *arg) {
  int accel_read = 0;
  while (LISTEN_ACCEL) {
    pair direction;
    get_direction(&direction);
    pair position = P1.position; // copia para fazer os calculos
    
    position.x += direction.x; // adciona o offset
    // verifica as bordas
    if (position.x > 620) position.x = 620;
    if (position.x < 0) position.x = 0;
    position.y += direction.y; // adiciona o offset
    // verifica as bordas
    if (position.y > 460) position.y = 460;
    if (position.y < 0) position.y = 0;

    // só movimenta o sprite se o personagem foi movimentado
    if (position.x != P1.position.x || position.y != P1.position.y){
      wbr_sp(1, P1.position.x, P1.position.y, P1.asset_id, P1.reg_id); 
    }

  }
  return NULL;
}
void stopAccelListener() { close_and_unmap_dev_mem(FD); }
