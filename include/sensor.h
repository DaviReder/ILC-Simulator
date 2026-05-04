#ifndef SENSOR_H_INCLUDED
#define SENSOR_H_INCLUDED

#include "models.h"

// Constantes de Negócio
#define TAM_HISTORICO 10
#define VALOR_INVALIDO -999.0f
#define MA_MIN 4.0f
#define MA_MAX 20.0f

// Interface de Gerenciamento
void carregar_configuracao(const char *caminho_arquivo);
void gerarleituraSensor(Sensor *s);
void atualizarValor(Sensor *s, int mA);
float mediaLeiturasSensor(Sensor *s);
float conversorAD(Sensor s);

// Interface de Exibição
void imprimirSensor();
void imprimirInformacoesSensor(Sensor *s);

#endif
