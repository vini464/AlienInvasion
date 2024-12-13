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
