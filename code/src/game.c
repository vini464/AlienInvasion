#include "../include/accel.h"
#include "../include/gpu.h"
#include "../include/mouse.h"
#include "../include/sprite_api.h"
#include "../include/utils.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define MAX_SHOTS 4
#define SHOT_SPEED 4
#define TOP 0
#define BOTTOM 480
#define LEFT 85
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
  Sprite sprite[2];            // cada jogador tem dois sprites
  Sprite shots[MAX_SHOTS];     // cada jogador tem no máximo 4 tiros;
  int active_shots[MAX_SHOTS]; // usado para verificar se o tiro está acionado
                               // ou não
  int avaiable_shots;          // limite de tiros disponíveis
  int life;
  int damage;
} Player;

extern unsigned short bg_inicio[4800];
extern unsigned short bg_jogo[4800];
extern unsigned short bg_alien_wins[4800];
extern unsigned short bg_human_wins[4800];
extern unsigned short bg_pause[4800];

// Movimentação do jogador 1
void *accelThread(void *arg);
void *buttonsThread(void *arg);

// Movimentação do jogador 2
void *mouseThread(void *arg);

// Gerencia os projéteis de ambos jogadores
void *projectilesThread(void *arg);

// Mostrar as informações na tela
void *renderThread(void *arg);

// Limpa a tela
void clear_screen();
void set_bg(unsigned short screen[4800]);

void game_set();
void timer(int trigger);

Player TANK, SHIP;
Sprite PORTAL1, PORTAL2;
int PAUSED, PAUSERENDER;
int END, NEW_GAME, WAIT;

int main(void) {
  gpu_open();
  game_set();

  pthread_t accel_t, mouse_t, buttons_t, projectiles_t, render_t;
  int stop_threads = 0;
  int portal_vel_1, portal_vel_2;
  PAUSERENDER = NEW_GAME = 0;
  WAIT = 1;
  pthread_create(&buttons_t, NULL, buttonsThread, &stop_threads);
  clear_screen();
  set_bg(bg_inicio);

  while (WAIT)
    ; // espera o jogador pressionar qualquer botão da placa

  pthread_create(&accel_t, NULL, accelThread, &stop_threads);
  pthread_create(&mouse_t, NULL, mouseThread, &stop_threads);
  pthread_create(&projectiles_t, NULL, projectilesThread, &stop_threads);
  pthread_create(&render_t, NULL, renderThread, &stop_threads);

  do {
    game_set();
    while (TANK.life > 0 && SHIP.life > 0) {
      if (PAUSED)
        continue;
      portal_vel_1 = 1, portal_vel_2 = -1;
      PORTAL1.pos_x += portal_vel_1;
      PORTAL2.pos_x += portal_vel_2;
      if (PORTAL1.pos_x > RIGHT) {
        PORTAL1.pos_x = LEFT;
      }
      if (PORTAL2.pos_x < LEFT) {
        PORTAL2.pos_x = RIGHT - 20;
      }
      usleep(10000);
      /**
            if (PAUSED) {
              printf("o jogo está pausado!\n");
              continue;
            } else {
              system("clear");
              printf("Jogador 1 [TANK]:\n");
              printf("Tiros disponíveis: %d\n", TANK.avaiable_shots);
              printf("Vida: %d\n", TANK.life);
              printf("Jogador 2 [SHIP]\n");
              printf("Tiros disponíveis: %d\n", SHIP.avaiable_shots);
              printf("Vida: %d\n", SHIP.life);
              printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
            }
      **/
    }
    printf("game over!\n");
    printf("Venceder: %s\n", TANK.life == 0 ? "SHIP" : "TANK");
    PAUSERENDER = 1;
    if (TANK.life <= 0) {
      set_bg(bg_alien_wins);
    }

    else {
      set_bg(bg_human_wins);
    }
    END = 1;
    while (END);

  } while (NEW_GAME);

  printf("game over\n");
  stop_threads = 1;
  pthread_join(accel_t, NULL);
  pthread_join(mouse_t, NULL);
  pthread_join(buttons_t, NULL);
  pthread_join(projectiles_t, NULL);
  pthread_join(render_t, NULL);
  clear_screen();
  gpu_close();

  return 0;
}

void timer(int trigger) {
  int msec = 0;
  clock_t difference, before = clock();
  do {
    difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;
  } while (msec < trigger);
}

// thread do acelerômetro [jogador2]
void *accelThread(void *arg) {
  int *stop = (int *)arg;

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
    if (PAUSED || END)
      continue;

    TANK.sprite[0].pos_x +=
        direction.x / 20; // só vai ser movimentado no eixo x
    TANK.sprite[1].pos_x +=
        direction.x / 20; // só vai ser movimentado no eixo x
    if (TANK.sprite[0].pos_x < LEFT || TANK.sprite[0].pos_x > RIGHT - 40) {
      TANK.sprite[0].pos_x -=
          direction.x / 20; // só vai ser movimentado no eixo x
      TANK.sprite[1].pos_x -=
          direction.x / 20; // só vai ser movimentado no eixo x
    }
    // timer(1); // 1 milisegundo
    usleep(10000);
  }

  close_and_unmap_dev_mem(fd);
  return NULL;
}

// leitura dos botões da placa, [jogador1]
void *buttonsThread(void *arg) {
  int *stop = (int *)arg;
  int btn, i, hold = 0;
  while (!(*stop)) {
    btn = ((~read_keys()) & 0b1111);
    if (WAIT && btn != 0) {
      WAIT = 0;
      btn = 0;
      hold = 1;
      continue;
    }
    switch (btn) {
    case 1: // tiro
      if (TANK.avaiable_shots && !hold && !END && !PAUSED) {
        hold = 1;
        TANK.avaiable_shots--;
        for (i = 0; i < MAX_SHOTS; i++) {
          if (!TANK.active_shots[i]) {
            TANK.active_shots[i] = 1;
            TANK.shots[i].act = 1;
            TANK.shots[i].pos_x = TANK.sprite[1].pos_x;
            TANK.shots[i].pos_y = TANK.sprite[0].pos_y;
            break;
          }
        }
      }
      break;
    case 2:
      if (!hold && !END) {
        hold = 1;
        if (PAUSED) {
          dp_square(WHITE, 0, 20, 20, 0);
          dp_square(WHITE, 0, 40, 20, 1);
          dp_square(WHITE, 0, 20, 50, 2);
          dp_square(WHITE, 0, 40, 50, 3);
          PAUSERENDER = 0;
          PAUSED = 0;
        } else {
          PAUSED = 1;
          PAUSERENDER = 1;
          dp_square(WHITE, 1, 20, 20, 0);
          dp_square(WHITE, 1, 40, 20, 1);
          dp_square(WHITE, 1, 20, 50, 2);
          dp_square(WHITE, 1, 40, 50, 3);
        }
      }

      break;
    case 4:
      if (!hold) {
        hold = 1;
        if (PAUSED && !END) {
          printf("reiniciando...\n");
          clear_screen();
          game_set();
          PAUSERENDER = 0;
        }
        if (END) {
          printf("reiniciando...\n");
          clear_screen();
          PAUSERENDER = 0;
          NEW_GAME = 1;
          END = 0;
        }
      }
      btn = 0;
      break;
    case 8:
      if (PAUSED || END) {
        PAUSERENDER = 1;
        clear_screen();
        exit(0);
      }
      btn = 0;
      break;
    default:
      hold = 0;
      btn = 0;
      break;
    }
  }
  return NULL;
}

// Controle do jogador 2 (ship)
void *mouseThread(void *arg) {
  int *stop = (int *)arg;
  int fd = open_mouse();
  int i;                   // auxiliar para percorrer os sprites
  mouse_event mouse = {0}; // pode dar erro no mouse.direction
  mouse.direction.x = 0;
  mouse.direction.y = 0;
  while (!(*stop)) {
    read_mouse(fd, &mouse);
    if (PAUSED || END) {
      mouse.btn_left = 0;
      mouse.btn_middle = 0; // TODO: habilitar lógica de reinicio
      continue;             // vai para a próxima iteração do while
    }
    if (mouse.btn_left) {
      if (SHIP.avaiable_shots > 0) {
        for (i = 0; i < MAX_SHOTS; i++) {
          if (!SHIP.active_shots[i]) {
            SHIP.active_shots[i] = 1;
            SHIP.shots[i].pos_x = SHIP.sprite[1].pos_x;
            SHIP.shots[i].pos_y = SHIP.sprite[0].pos_y + 20;
            SHIP.shots[i].act = 1;
            SHIP.avaiable_shots--;
            break;
          }
        }
      }
      mouse.btn_left = 0;
    }

    for (i = 0; i < 2; i++) {
      SHIP.sprite[i].pos_x += mouse.direction.x;
      SHIP.sprite[i].pos_y += mouse.direction.y;
      if (SHIP.sprite[i].pos_y < TOP)
        SHIP.sprite[i].pos_y = TOP;
      if (SHIP.sprite[i].pos_y >
          BOTTOM / 3 - 20) // TODO: trocar 20 por tamanho do sprite
        SHIP.sprite[i].pos_y = BOTTOM / 3 - 20;
      if (SHIP.sprite[0].pos_x <= LEFT) {
        SHIP.sprite[0].pos_x = LEFT;
        SHIP.sprite[1].pos_x = LEFT + 20;
      }
      if (SHIP.sprite[1].pos_x > RIGHT - 20) {
        SHIP.sprite[0].pos_x = RIGHT - 40;
        SHIP.sprite[1].pos_x = RIGHT - 20;
      }
    }
  }

  close_mouse(fd);
  return NULL;
}

void *projectilesThread(void *arg) {
  int *stop = (int *)arg;
  int i, tp, collide = 0;
  while (!(*stop)) {
    while (PAUSED)
      ;
    for (i = 0; i < MAX_SHOTS; i++) {
      if (TANK.active_shots[i] && !PAUSED) {
        TANK.shots[i].pos_y -= SHOT_SPEED;
        TANK.shots[i].mem_offset++;
        if (TANK.shots[i].mem_offset > 11)
          TANK.shots[i].mem_offset = 9;
        tp = checkCollision(&TANK.shots[i], &PORTAL2);
        if (tp) {
          TANK.shots[i].pos_x = PORTAL1.pos_x;
          TANK.shots[i].pos_y = PORTAL1.pos_y - 10;
        }
        collide = checkCollision(&TANK.shots[i], &SHIP.sprite[0]) ||
                  checkCollision(&TANK.shots[i], &SHIP.sprite[1]);
        if (!PAUSED && (collide || TANK.shots[i].pos_y < TOP)) {
          TANK.active_shots[i] = 0;
          TANK.avaiable_shots++;
          TANK.shots[i].act = 0;
          if (collide) {
            SHIP.life -= TANK.damage;
          }
        }
      }
      if (!PAUSED && SHIP.active_shots[i]) {
        SHIP.shots[i].pos_y += SHOT_SPEED;
        tp = checkCollision(&SHIP.shots[i], &PORTAL1);
        if (tp) {
          SHIP.shots[i].pos_x = PORTAL2.pos_x;
          SHIP.shots[i].pos_y = PORTAL2.pos_y + 10;
        }
        collide = checkCollision(&SHIP.shots[i], &TANK.sprite[0]) ||
                  checkCollision(&SHIP.shots[i], &TANK.sprite[1]);
        if (!PAUSED &&
            (collide ||
             SHIP.shots[i].pos_y >
                 BOTTOM -
                     20)) { // TODO: alterar constante 20 para um valor variável
          // dentro da sprite, fazer isso tambem com TOP

          SHIP.active_shots[i] = 0;
          SHIP.avaiable_shots++;
          SHIP.shots[i].act = 0;
          if (collide) {
            TANK.life -= SHIP.damage;
          }
        }
      }
    }
    if (!PAUSED)
      timer(4); // espera por 4 milisegundos
  }
  return NULL;
}

void *renderThread(void *arg) {
  int *stop = (int *)arg;
  int i;
  while (!(*stop)) {
    if (!PAUSERENDER) {
      showSprite(&PORTAL1);
      showSprite(&PORTAL2);
      for (i = 0; i < 2; i++) {
        showSprite(&TANK.sprite[i]);
        showSprite(&SHIP.sprite[i]);
      }
      for (i = 0; i < MAX_SHOTS; i++) {
        showSprite(&TANK.shots[i]);
        showSprite(&SHIP.shots[i]);
      }
      // rendenizacao dos sprites de vida
      int base = 15; // onde comecam a sequencia de sprites dos numeros
      unsigned int decimal_tank = TANK.life / 10;
      unsigned int inteiro_tank = TANK.life % 10;
      unsigned int decimal_ship = SHIP.life / 10;
      unsigned int inteiro_ship = SHIP.life % 10;
      // tenho que ter o sprite base do numero
      if (TANK.life >= 10) {
        wbr_sp(1, 0, 460, base + decimal_tank, 17);
        wbr_sp(1, 25, 460, base + inteiro_tank, 18);
      } else {
        wbr_sp(0, 0, 460, base + decimal_tank, 17);
        wbr_sp(1, 25, 460, base + inteiro_tank, 18);
      }
      wbr_sp(1, 50, 460, base - 2, 19);
      wbr_sp(1, 50, 10, base - 1, 20);
      if (SHIP.life >= 10) {
        // aqui funciona pra qualquer valor da vida da nave com 2 digitos
        wbr_sp(1, 0, 10, base + decimal_ship, 16);
        wbr_sp(1, 25, 10, base + inteiro_ship, 15);
      } else {
        wbr_sp(0, 0, 10, base + decimal_ship, 16);
        wbr_sp(1, 25, 10, base + inteiro_ship, 15);
      }
    }
  }
  return NULL;
}

void clear_screen() {
  int i;
  timer(4);
  wbr_bg(0); // bg preto
  // limpar sprite regs
  for (i = 1; i < 32; i++) {
    wbr_sp(0, 0, 0, 0, i);
  }
  for (i = 1; i < 4800; i++) {
    wbm(510, i);
  }
  for (i = 0; i < 16; i++) {
    dp_square(510, 0, 0, 0, i);
  }
  set_hex(0, 0);
}

void game_set() {
  PAUSERENDER = 1;
  clear_screen();
  set_bg(bg_jogo);
  PAUSERENDER = 0;
  int i;
  // inicializa os jogadores
  TANK.damage = 1;
  TANK.life = 12;
  TANK.avaiable_shots = MAX_SHOTS;

  SHIP.damage = 1;
  SHIP.life = 5;
  SHIP.avaiable_shots = MAX_SHOTS;

  // TODO: diminuir um pixel de espaco entre os sprites
  for (i = 0; i < 2; i++) {
    TANK.sprite[i].reg_id = i + 1;
    TANK.sprite[i].act = 1;
    TANK.sprite[i].mem_offset = 4 + i; // TODO: alterar
    TANK.sprite[i].pos_x = CENTER + 20 * (i - 1);
    TANK.sprite[i].pos_y = BOTTOM - 20;

    SHIP.sprite[i].reg_id = i + 3;
    SHIP.sprite[i].act = 1;
    SHIP.sprite[i].mem_offset = i; // TODO: alterar
    SHIP.sprite[i].pos_y = 0;
    SHIP.sprite[i].pos_x = CENTER + 20 * (i - 1);
  }

  // inicializa os tiros
  for (i = 0; i < MAX_SHOTS; i++) {
    SHIP.shots[i].act = 0;
    SHIP.shots[i].reg_id = i + 11;
    SHIP.shots[i].mem_offset = 12; // DONE: alterado
    SHIP.shots[i].pos_y = 0;
    SHIP.shots[i].pos_x = 0;

    TANK.shots[i].act = 0;
    TANK.shots[i].reg_id = i + 7;
    TANK.shots[i].mem_offset = 9; // DONE: alterado
    TANK.shots[i].pos_y = 0;
    TANK.shots[i].pos_x = 0;
  }
  PORTAL1.act = 1;
  PORTAL1.reg_id = 22;
  PORTAL1.mem_offset = 25;
  PORTAL1.pos_y = BOTTOM / 2 - 20;
  PORTAL1.pos_x = RIGHT / 2;

  PORTAL2.act = 1;
  PORTAL2.reg_id = 21;
  PORTAL2.mem_offset = 26;
  PORTAL2.pos_y = BOTTOM / 2 + 20;
  PORTAL2.pos_x = RIGHT / 2;

  END = 0;
  PAUSED = 0;
}

void set_bg(unsigned short screen[4800]) {
  int i = 0;
  for (i = 0; i < 4800; i++) {
    wbm(screen[i], i);
  }
}
