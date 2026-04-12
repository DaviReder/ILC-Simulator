#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED
#include "../include/models.h"
#include "../include/arvore.h"

//Variaveis de BITWISE.
#define SHIFT_DIGITAL 0
#define SHIFT_LUZ 8
#define SHIFT_CLIMA 12 // Temp e Umid
#define MASCARA_CLIMA 0x07

unsigned int analogLuz(unsigned int saida, float luminosidade);
unsigned int analogTemp(unsigned int saida, float temperatura, float umidade);
int executaSimulacaoPLC();
void monitorarSaidas(unsigned int saida);
void* cicloControleAtuadores(void* arg);

#endif // CONTROLLER_H_INCLUDED
