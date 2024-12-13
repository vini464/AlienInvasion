#include "../include/utils.h"
#include "../include/mouse.h"
#include "../include/accel.h"
#include "../include/gpu.h"
#include "../include/sprite_api.h"

#include <time.h>

#define MAX_SHOTS 4
#define SHOT_SPEED 4
#define TOP 0
#define BOTTOM 480
#define LEFT 0
#define RIGHT 640

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

int main(void) {


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

// Controle do jogador 2 (ship)
void * mouseThread(void * arg) {
  int * stop = (int *) arg;
  int fd = open_mouse();
  int i; // auxiliar para percorrer os sprites
  mouse_event mouse ={0};
  while (!(*stop)) {
    read_mouse(fd, &mouse);
    if (mouse.btn_left) {
      if (SHIP.avaiable_shots > 0) {
        for (i=0; i<MAX_SHOTS; i++) {
          if (!SHIP.active_shots[i]) {
            SHIP.active_shots[i] = 1;
            SHIP.shots[i].pos_x = SHIP.sprite[1].pos_x;
            SHIP.shots[i].pos_y = SHIP.sprite[0].pos_y;
            SHIP.shots[i].act = 1;
            SHIP.shots[i].reg_id = i+11; // registrador fixo
            SHIP.shots[i].mem_offset = 11; // TODO: atualizar este valor
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
      if (TANK.active_shots[i]) {
        showSprite(&TANK.shots[i]);
      }
      if (SHIP.active_shots[i]) {
        showSprite(&SHIP.shots[i]);
      }
    }
  }
  return NULL;
}
