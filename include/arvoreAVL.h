#ifndef ARVORE_H_INCLUDED
#define ARVORE_H_INCLUDED
#include "models.h"

// Interface Pública da Árvore
No* inserirArvoreAVL(No *raiz, Sensor s);
No* buscarArvore(No *raiz, int id);
Sensor* buscarSensor(No *raiz, int id);
void imprimirArvore(No *raiz, int nivel);
No* liberarArvore(No *raiz);

#endif
