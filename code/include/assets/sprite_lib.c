#include <iso646.h>
#include <stdint.h>
#include <stdio.h>

#define TOTAL 400
#define N_DIV 40
#define SUB 400

extern const uint32_t nave_d[1][1600];
extern const uint32_t nave_dir[1][1600];
extern const uint32_t nave_esq[1][1600];
extern const uint32_t tank_clean[1][1600];
extern const uint32_t tank_data1[1][1600];
extern const uint32_t tank_data2[1][1600];
extern const uint32_t vaca_data[1][400];
extern const uint32_t tiro_data[1][400];

uint8_t reduzirPara3Bits(uint8_t valor) { return (valor * 7 + 127) / 255; }

void dividirSprite(const uint32_t array[1][TOTAL], uint32_t m1[SUB], uint32_t m2[SUB],
                   uint32_t m3[SUB], uint32_t m4[SUB]) {
  int i;
  int j;
  int index_m1 = 0;
  int index_m2 = 0;
  int index_m3 = 0;
  int index_m4 = 0;
  int index_total = 0;
  for (i = 0; i < 40; i++) {
    for (j = 0; j < 40; j++) {

      if (i < 20 and j < 20) {
        m1[index_m1] = array[0][index_total];
        index_m1++;
      }

      else if (i < 20 and j >= 20) {
        m2[index_m2] = array[0][index_total];
        index_m2++;
      }

      else if (i >= 20 and j < 20) {
        m3[index_m3] = array[0][index_total];
        index_m3++;
      }

      else if (i >= 20 and j >= 20) {
        m4[index_m4] = array[0][index_total];
        index_m4++;
      }
      //  printf("index_total: %d\n", index_total);
      //  printf("I,J: %d %d ", i,j);
      index_total++;
    }
  }
}

void processarMatriz(uint32_t matriz[400]) {
  int i, valor;
  for (i = 0; i < 400; i++) {
    valor = matriz[i];
    // Extrair RR, GG e BB
    uint8_t r = (valor >> 16) & 0xFF; // Bits 16-23
    uint8_t g = (valor >> 8) & 0xFF;  // Bits 8-15
    uint8_t b = valor & 0xFF;         // Bits 0-7

    // Reduzir para 3 bits
    r = reduzirPara3Bits(r);
    g = reduzirPara3Bits(g);
    b = reduzirPara3Bits(b);
    matriz[i] = (b << 6) | (g << 3) | r;
  }
}

void print_sprite(uint32_t matriz[400], char *name) {
  int i;
  printf("short %s[400] = {", name);

  for (i = 0; i < 400; i++) {
    if (i != 399) {

      printf("%u, ", matriz[i]);
    } else {
      printf("%u}; \n", matriz[i]);
    }
  }
  printf("\n");
}

int main() {

  uint32_t m1[400], m2[400], m3[400], m4[400];
  dividirSprite(vaca_data, m1, m2, m3, m4);
  processarMatriz(m1);
  print_sprite(m1, "cow");
  dividirSprite(tiro_data, m1, m2, m3, m4);
  processarMatriz(m1);
  print_sprite(m1, "shoot");
  return 0;
}



