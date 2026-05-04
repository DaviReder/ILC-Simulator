#ifndef MODELS_H_INCLUDED
#define MODELS_H_INCLUDED

extern volatile int rodando_sistema;

typedef struct {
    int id;                // Chave da Árvore
    char tag[10];          // Ex: "TEMP_01"
    float range_min;       // Ex: 4 (mA) -> 15.0 (°C)
    float range_max;       // Ex: 20 (mA) -> 45.0 (°C)
    float leitura_atual;   // Valor real após Regra de 3
    float historico[10];   // Últimas 10 leituras (Circular)
    int pos_hist;          // Índice do array circular
} Sensor;

typedef struct no{
    Sensor sensor;
    struct no *esquerda, *direita;
    short altura;
}No;

#endif // MODELS_H_INCLUDED
