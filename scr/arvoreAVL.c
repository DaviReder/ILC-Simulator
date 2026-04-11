#include <stdio.h>
#include <stdlib.h>
#include "../include/arvore.h"

No* criarNo(Sensor s){
    No *novo = malloc(sizeof(No));

    if(novo){
        novo->sensor = s;
        novo->esquerda = NULL;
        novo->direita = NULL;
        novo->altura = 0;
        return novo;
    }
    else
        printf("\nErro ao alocar memoria.\n");
    return NULL;
}

short maior(short a, short b){
    if(a > b)
        return a;
    else
        return b;
}

short getAltura(No *no){
    if(no == NULL){
        return -1;
    }
    return no->altura;
}

short fatorBalanceamento(No *no){
    if(no == NULL)
        return 0;
    return (getAltura(no->esquerda) - getAltura(no->direita));

}

No* rotacionarDireita(No *no){
    No *y, *f;

    y = no->esquerda;
    f = y->direita;

    y->direita = no;
    no->esquerda = f;

    no->altura = maior(getAltura(no->esquerda), getAltura(no->direita)) +1;
    y->altura = maior(getAltura(y->esquerda), getAltura(y->direita)) +1;

    return y;
}

No* rotacionarEsquerda(No *no){
    No *y, *f;

    y = no->direita;
    f = y->esquerda;

    y->esquerda = no;
    no->direita = f;

    no->altura = maior(getAltura(no->esquerda), getAltura(no->direita)) +1;
    y->altura = maior(getAltura(y->esquerda), getAltura(y->direita)) +1;

    return y;
}

No* rotacaoDirEsq(No *no){
    no->direita = rotacionarDireita(no->direita);
    return rotacionarEsquerda(no);
}

No* rotacaoEsqDir(No *no){
    no->esquerda = rotacionarEsquerda(no->esquerda);
    return rotacionarDireita(no);
}

No* balancearAVL(No *raiz){
    short fb = fatorBalanceamento(raiz);

    if(fb < -1 && fatorBalanceamento(raiz->direita) <= 0)
        raiz = rotacionarEsquerda(raiz);

    // Rotação à Direita (Esquerda-Esquerda)
    else if(fb > 1 && fatorBalanceamento(raiz->esquerda) >= 0)
        raiz = rotacionarDireita(raiz);

    // Rotação Dupla à Direita (Esquerda-Direita)
    else if(fb > 1 && fatorBalanceamento(raiz->esquerda) < 0)
        raiz = rotacaoEsqDir(raiz);

    // Rotação Dupla à Esquerda (Direita-Esquerda)
    else if(fb < -1 && fatorBalanceamento(raiz->direita) > 0)
        raiz = rotacaoDirEsq(raiz);

    return raiz;
}

No* inserirArvoreAVL(No *raiz, Sensor s){
    No *aux = raiz;
    if(raiz == NULL){
        return criarNo(s);
    }

    if(s.id > raiz->sensor.id){
        raiz->direita = inserirArvoreAVL(raiz->direita, s);
    }
    else if(s.id < raiz->sensor.id){
        raiz->esquerda = inserirArvoreAVL(raiz->esquerda, s);
    }
    else{
        printf("\nEsse sensor já existe! ID: %d.\n", s.id);
    }

    //Precisa calcular a altura e efetuar o balanceamento
    raiz->altura = maior(getAltura(raiz->esquerda), getAltura(raiz->direita)) + 1;
    return balancearAVL(raiz);
}

void imprimirArvore(No *raiz, int nivel){
    if(raiz){
        imprimirArvore(raiz->direita, nivel + 1);
        printf("\n\n");
        for(int i = 0; i < nivel; i++) printf("\t");
        printf("%d", raiz->sensor.id);
        imprimirArvore(raiz->esquerda, nivel + 1);
    }
}

No* buscarArvore(No *raiz, int id){
    if(raiz == NULL) return NULL;
    if(id == raiz->sensor.id) return raiz;

    No *aux = raiz;
    if(id > raiz->sensor.id)
        aux = buscarArvore(aux->direita, id);
    else if(id < raiz->sensor.id)
        aux = buscarArvore(aux->esquerda, id);
    return aux;
}

Sensor* buscarSensor(No *raiz, int id){
    if(raiz == NULL) return NULL;
    if(id == raiz->sensor.id) return &(raiz->sensor);

    if(id > raiz->sensor.id)
        return buscarSensor(raiz->direita, id);
    else
        return buscarSensor(raiz->esquerda, id);
}

