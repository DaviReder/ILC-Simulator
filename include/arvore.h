#ifndef ARVORE_H_INCLUDED
#define ARVORE_H_INCLUDED
#include "../models.h"

typedef struct no{
    Sensor sensor;
    struct no *esquerda, *direita;
    short altura;
}No;

//PARA LEITURA DO SENSOR:
extern No *raiz_sensores; // Raiz está em outro arquivo.
void carregar_configuracao(const char *caminho_arquivo);

//ARVORE.C
No* criarNo(Sensor s);
short maior(short a, short b);
short getAltura(No *no);
short fatorBalanceamento(No *no);
No* rotacionarEsquerda(No *no);
No* rotacionarDireita(No *no);
No* rotacaoDirEsq(No *no);
No* rotacaoEsqDir(No *no);
No* balancearAVL(No *raiz);
No* inserirArvoreAVL(No *raiz, Sensor s);
void imprimirArvore(No *raiz, int nivel);
No* buscarArvore(No *raiz, int id);
Sensor* buscarSensor(No *raiz, int id);

//SENSOR.C
float conversorAD(Sensor s);
void gerarleituraSensor(Sensor *s);
void imprimirSensor();
float mediaLeiturasSensor(Sensor *s);
void imprimirInformacoesSensor(Sensor *s);

#endif // ARVORE_H_INCLUDED
