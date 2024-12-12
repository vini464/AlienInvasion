#include "../include/utils.h"
#include "../include/mouse.h"
#include "../include/accel.h"
#include "../include/gpu.h"
#include "../include/sprite_api.h"


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
  Sprite shots[4]; // cada jogador tem no máximo 4 tiros;
  int active_shots[4]; // usado para verificar se o tiro está acionado ou não
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



void * renderThread(void * arg) {
  int *stop = (int*) arg;
  while (!(*stop)) {
    showSprite(&TANK.sprite[0]);
    showSprite(&TANK.sprite[1]);
    showSprite(&SHIP.sprite[0]);
    showSprite(&SHIP.sprite[1]);
    int i;
    for (i = 0; i < 2; i++) {
      showSprite(&TANK.sprite[i]);
      showSprite(&SHIP.sprite[i]);
    }
    for (i = 0; i < 4; i++) {
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
