#include "../include/utils.h"
#include "../include/mouse.h"
#include "../include/accel.h"
#include "../include/gpu.h"
#include "../include/sprite_api.h"

#include <time.h>
#include <stdio.h>

#define MAX_SHOTS 4
#define SHOT_SPEED 4
#define TOP 0
#define BOTTOM 480
#define LEFT 0
#define RIGHT 640
#define CENTER 320

/*
 * Sobre os registradores:
 * 1 - 2 -> reservados para o jogador 1
 * 3 - 4 -> reservados para o jogador 2
 * 5 - 6 -> reservados para o cronômetro
 * 7 - 10 -> reservados para os projéteis do jogador 1
 * 11 - 14 -> reservados para os projéteis do jogador 2
 * 
 */



typedef struct player {
  Sprite sprite[2]; // cada jogador tem dois sprites 
  Sprite shots[MAX_SHOTS]; // cada jogador tem no máximo 4 tiros;
  int active_shots[MAX_SHOTS]; // usado para verificar se o tiro está acionado ou não
  int avaiable_shots; // limite de tiros disponíveis
  int life;
  int damage;
} Player;



// Movimentação do jogador 1
void * accelThread(void * arg);
void * buttonsThread(void * arg);

// Movimentação do jogador 2
void * mouseThread(void * arg);

// Gerencia os projéteis de ambos jogadores
void * projectilesThread(void * arg);

// Mostrar as informações na tela
void * renderThread(void * arg);

Player TANK, SHIP;
int PAUSED;

int main(void) {
  int i;
  // inicializa os jogadores
  for (i=0; i< 2; i++) {
    TANK.sprite[i].reg_id = i + 1;
    TANK.sprite[i].act = 1;
    TANK.sprite[i].mem_offset = i; // TODO: alterar
    TANK.sprite[i].pos_x = CENTER + 20 * (i - 1);
    TANK.sprite[i].pos_y = BOTTOM-20;

    SHIP.sprite[i].reg_id = i + 3;
    SHIP.sprite[i].act = 1;
    SHIP.sprite[i].mem_offset = i + 2; // TODO: alterar
    SHIP.sprite[i].pos_y = 0;
    SHIP.sprite[i].pos_x = CENTER + 20 * (i - 1) ;

  }
  // inicializa os tiros
  for (i=0;i<MAX_SHOTS;i++) {
    SHIP.shots[i].act = 0;
    SHIP.shots[i].reg_id = i + 11;
    SHIP.shots[i].mem_offset = 11; // TODO: alterar
    SHIP.shots[i].pos_y = 0;
    SHIP.shots[i].pos_x = 0;

    TANK.shots[i].act = 0;
    TANK.shots[i].reg_id = i + 7;
    TANK.shots[i].mem_offset = 11; // TODO: alterar
    TANK.shots[i].pos_y = 0;
    TANK.shots[i].pos_x = 0;

  }
    

  return 0;
}


void timer(int trigger) {
  int msec =0;
  clock_t difference, before = clock();
  do {
    difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;
  } while (msec < trigger);
}

// thread do acelerômetro [jogador2]
void * accelThread(void * arg) {
  int * stop = (int *) arg;

  int i, fd;

  for (i = 0; i < 10; i++) {
    fd = open_and_mmap_dev_mem();
    if (fd == -1)
      printf("não foi possível abrir /dev/mem\n");
    else
      break;
  }
  if (fd == -1)
    exit(-1);
  I2C0_init();
  accel_init();
  accel_calibrate(60);
  
  pair direction;
  while (!(*stop)) {
    get_direction(&direction);
    for (i = 0; i < 2; i++) {
      TANK.sprite[i].pos_x += direction.x; // só vai ser movimentado no eixo x
      if (checkCollision(&TANK.sprite[i], &SHIP.sprite[0]) || checkCollision(&TANK.sprite[i], &SHIP.sprite[1])) {
        TANK.sprite[i].pos_x -= direction.x;
      }
      if (TANK.sprite[i].pos_x < LEFT + i * 20) 
        TANK.sprite[i].pos_x = LEFT + i * 20; 
      else if (TANK.sprite[i].pos_x > RIGHT + 20 * (i-1))
        TANK.sprite[i].pos_x = RIGHT + 20 * (i-1);
    }
  }

  close_and_unmap_dev_mem(fd);
  return NULL;
}

// leitura dos botões da placa, [jogador1]
void * buttonsThread(void * arg) {
  int * stop = (int *) arg;
  int btn, i, hold=0;
  while (!(*stop)) {
    btn = (~read_keys()) & 0b1111;
    switch (btn) {
      case 1: // tiro
        printf("tiros disponíveis: %d\n", TANK.avaiable_shots);
        if (TANK.avaiable_shots && !hold) {
          hold = 1;
          TANK.avaiable_shots--;
          for (i=0; i<MAX_SHOTS; i++) {
            if (!TANK.active_shots[i]) {
              TANK.active_shots[i] = 1;
              TANK.shots[i].act = 1;
              TANK.shots[i].pos_x = TANK.sprite[1].pos_x;
              TANK.shots[i].pos_y = TANK.sprite[0].pos_y;
            }
          }
        }
        break;
      case 2:
        printf("botão 2");
        break;
      case 4:
        printf("botão 3");
        break;
      case 8:
        printf("botão 4");
        break;
      default:
        hold = 0;
        break;
    }
  }
  return NULL;
}


// Controle do jogador 2 (ship)
void * mouseThread(void * arg) {
  int * stop = (int *) arg;
  int fd = open_mouse();
  int i; // auxiliar para percorrer os sprites
  mouse_event mouse ={0}; // pode dar erro no mouse.direction
  while (!(*stop)) {
    read_mouse(fd, &mouse);
    if (mouse.btn_right) { // pausa o jogo
      if (PAUSED)
        PAUSED = 0;
      else 
        PAUSED = 1;
      mouse.btn_right = 0;
    }
    if (PAUSED) {
      mouse.btn_left = 0;
      mouse.btn_middle = 0; // TODO: habilitar lógica de reinicio
      continue; // vai para a próxima iteração do while
    }
    if (mouse.btn_left) {
      if (SHIP.avaiable_shots > 0) {
        for (i=0; i<MAX_SHOTS; i++) {
          if (!SHIP.active_shots[i]) {
            SHIP.active_shots[i] = 1;
            SHIP.shots[i].pos_x = SHIP.sprite[1].pos_x;
            SHIP.shots[i].pos_y = SHIP.sprite[0].pos_y + 20;
            SHIP.shots[i].act = 1;
            break;
          }
        }
      }
      mouse.btn_left = 0;
    }
    for (i=0; i<2; i++) {
      SHIP.sprite[i].pos_x += mouse.direction.x;
      if (checkCollision(&SHIP.sprite[i], &TANK.sprite[0]) || checkCollision(&SHIP.sprite[i], &TANK.sprite[1])){
        SHIP.sprite[i].pos_x -= mouse.direction.x;
      }
      SHIP.sprite[i].pos_y -= mouse.direction.y;
      if (checkCollision(&SHIP.sprite[i], &TANK.sprite[0]) || checkCollision(&SHIP.sprite[i], &TANK.sprite[1])){
        SHIP.sprite[i].pos_y += mouse.direction.y;
      }
      if (SHIP.sprite[i].pos_y < TOP) 
        SHIP.sprite[i].pos_y = TOP;
      else if (SHIP.sprite[i].pos_y > BOTTOM - 20) // TODO: trocar 20 por tamanho do sprite
        SHIP.sprite[i].pos_y = TOP;
      else if (SHIP.sprite[i].pos_x < LEFT + i *20)
        SHIP.sprite[i].pos_x = LEFT + i*20;
      else if (SHIP.sprite[i].pos_x > RIGHT - (20 * i) + 40)
        SHIP.sprite[i].pos_x = RIGHT + RIGHT - (20 * i) + 40;
    }
  }
  close_mouse(fd);
  return NULL;
}

void * projectilesThread(void * arg) {
  int *stop = (int *)arg;
  int i;
  while (!(*stop)) {
    for (i=0; i<MAX_SHOTS; i++) {
      if (TANK.active_shots[i]){
        TANK.shots[i].pos_y -= SHOT_SPEED;
        if (checkCollision(&TANK.shots[i], &SHIP.sprite[0]) || checkCollision(&TANK.shots[i], &SHIP.sprite[0]) || TANK.shots[i].pos_y < TOP){
          TANK.active_shots[i] = 0;
          TANK.avaiable_shots++;
          TANK.shots[i].act = 0;
          if (TANK.shots[i].pos_y >= TOP){
            SHIP.life -= TANK.damage;
          }
        }
      }
      if (SHIP.active_shots[i]){
        SHIP.shots[i].pos_y += SHOT_SPEED;
        if (checkCollision(&SHIP.shots[i], &TANK.sprite[0]) || checkCollision(&SHIP.shots[i], &TANK.sprite[0]) || SHIP.shots[i].pos_y > BOTTOM - 20){ // TODO: alterar constante 20 para um valor variável dentro da sprite, fazer isso tambem com TOP 

          SHIP.active_shots[i] = 0;
          SHIP.avaiable_shots++;
          SHIP.shots[i].act = 0;
          if (SHIP.shots[i].pos_y <= BOTTOM - 20){
            TANK.life -= SHIP.damage;
          }
        }
      }
    }
    timer(4); // espera por 4 milisegundos
  }
  return NULL;
}



void * renderThread(void * arg) {
  int *stop = (int*) arg;
  int i;
  while (!(*stop)) {
    for (i = 0; i < 2; i++) {
      showSprite(&TANK.sprite[i]);
      showSprite(&SHIP.sprite[i]);
    }
    for (i = 0; i < MAX_SHOTS; i++) {
        showSprite(&TANK.shots[i]);
        showSprite(&SHIP.shots[i]);
    }
  }
  return NULL;
}
