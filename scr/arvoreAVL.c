#include <stdio.h>
#include <stdlib.h>
#include "../include/arvoreAVL.h"
#include "../include/log.h"

// --- Funções Auxiliares (Privadas) ---

static short maior(short a, short b) {
    return (a > b) ? a : b;
}

static short getAltura(No *no) {
    return (no == NULL) ? -1 : no->altura;
}

static void atualizarAltura(No *no) {
    if (no != NULL) {
        no->altura = maior(getAltura(no->esquerda), getAltura(no->direita)) + 1;
    }
}

short fatorBalanceamento(No *no) {
    return (no == NULL) ? 0 : (getAltura(no->esquerda) - getAltura(no->direita));
}

// --- Rotações ---

No* rotacionarDireita(No *no) {
    No *novaRaiz = no->esquerda;
    No *subArvoreDir = novaRaiz->direita;

    novaRaiz->direita = no;
    no->esquerda = subArvoreDir;

    atualizarAltura(no);
    atualizarAltura(novaRaiz);

    return novaRaiz;
}

No* rotacionarEsquerda(No *no) {
    No *novaRaiz = no->direita;
    No *subArvoreEsq = novaRaiz->esquerda;

    novaRaiz->esquerda = no;
    no->direita = subArvoreEsq;

    atualizarAltura(no);
    atualizarAltura(novaRaiz);

    return novaRaiz;
}

// --- Lógica de Balanceamento ---

No* balancearAVL(No *raiz) {
    short fb = fatorBalanceamento(raiz);

    // Caso Esquerda-Esquerda ou Esquerda-Direita
    if (fb > 1) {
        if (fatorBalanceamento(raiz->esquerda) < 0) {
            raiz->esquerda = rotacionarEsquerda(raiz->esquerda);
        }
        return rotacionarDireita(raiz);
    }

    // Caso Direita-Direita ou Direita-Esquerda
    if (fb < -1) {
        if (fatorBalanceamento(raiz->direita) > 0) {
            raiz->direita = rotacionarDireita(raiz->direita);
        }
        return rotacionarEsquerda(raiz);
    }

    return raiz;
}

// --- Interface Principal ---

No* criarNo(Sensor s) {
    No *novo = malloc(sizeof(No));

    if (!novo) {
        log_evento("Erro", "Falha crítica: Alocação de memória para No falhou.");
        return NULL;
    }

    novo->sensor = s;
    novo->esquerda = NULL;
    novo->direita = NULL;
    novo->altura = 0;
    return novo;
}

No* inserirArvoreAVL(No *raiz, Sensor s) {
    if (raiz == NULL) return criarNo(s);

    if (s.id < raiz->sensor.id) {
        raiz->esquerda = inserirArvoreAVL(raiz->esquerda, s);
    }
    else if (s.id > raiz->sensor.id) {
        raiz->direita = inserirArvoreAVL(raiz->direita, s);
    }
    else {
        log_evento("AVISO", "ID de sensor duplicado ignorado.");
        return raiz;
    }

    atualizarAltura(raiz);
    return balancearAVL(raiz);
}

No* buscarArvore(No *raiz, int id) {
    if (raiz == NULL || raiz->sensor.id == id) return raiz;

    if (id < raiz->sensor.id)
        return buscarArvore(raiz->esquerda, id);

    return buscarArvore(raiz->direita, id);
}

Sensor* buscarSensor(No *raiz, int id) {
    No *no = buscarArvore(raiz, id);
    return (no != NULL) ? &(no->sensor) : NULL;
}

void imprimirArvore(No *raiz, int nivel) {
    if (raiz == NULL) return;

    imprimirArvore(raiz->direita, nivel + 1);

    printf("\n");
    for (int i = 0; i < nivel; i++) printf("\t");
    printf("%d", raiz->sensor.id);

    imprimirArvore(raiz->esquerda, nivel + 1);
}

// --- Desalocação de Memória ---

No* liberarArvore(No *raiz) {
    if (raiz == NULL) {
        return NULL;
    }

    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);

    free(raiz);
    return NULL;
}
