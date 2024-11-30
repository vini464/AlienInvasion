#include <stdio.h>
#include <stdint.h>
#include <iso646.h>

#define TOTAL 1600
#define N_DIV 40
#define SUB 400

void dividirSprite(int array[1][TOTAL], int m1[SUB],int m2[SUB],int m3[SUB],int m4[SUB]){
    int i;
    int j;
    int index_m1,index_m2,index_m3,index_m4,index_total = 0;
    for (i = 0; i < 40; i++) {
        for (j = 0; j < 40; j++) {

            if (i < 20 and j < 20){
                m1[index_m1] = array[0][index_total];
                index_m1++;

            }
            else if (i < 20 and j >= 20){
                m2[index_m2] = array[0][index_total];
                index_m2++;
            }

            else if (i >= 20 and j < 20){
                m3[index_m3] = array[0][index_total];
                index_m3++;
            }

            else if (i >= 20 and j >= 20){
                m4[index_m4] = array[0][index_total];
                index_m4++;
            }
            index_total++;
        }
    }
}


uint8_t reduzirPara3Bits(uint8_t valor) {
    return valor >> 5;
}

void processarMatriz(uint32_t matriz[400]) {
    int i, valor,rr,gg,bb;
     for (i = 0; i < 400; i++){
        valor = matriz[i];
        // Extrair RR, GG e BB
        uint8_t r = (valor >> 16) & 0xFF; // Bits 16-23
        uint8_t g = (valor >> 8) & 0xFF;  // Bits 8-15
        uint8_t b = valor & 0xFF;        // Bits 0-7

        // Reduzir para 3 bits
        r = reduzirPara3Bits(r);
        g = reduzirPara3Bits(g);
        b = reduzirPara3Bits(b);
        matriz[i] = (b << 6) | (g << 3) | r;
     }
}

void print_sprite(uint32_t matriz[400]){
    int i;
    for (i = 0; i < 400; i++){
        if (i != 399){
            printf(matriz[i]+", ");
        } else {
            printf(matriz[i])
        }
    }
}

int main(){
    int m1[400],m2[400],m3[400],m4[400];
    dividirSprite(,m1,m2,m3,m4);
    processarMatriz(m3);
    processarMatriz(m4);
}
