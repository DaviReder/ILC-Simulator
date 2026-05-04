#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../include/arvoreAVL.h"
#include "../include/sensor.h"
#include "../include/log.h"

extern No *raiz_sensores;

// --- Funções Internas ---

static void inicializar_dados_sensor(Sensor *s) {
    s->leitura_atual = 0.0f;
    s->pos_hist = 0;
    for (int i = 0; i < TAM_HISTORICO; i++) {
        s->historico[i] = VALOR_INVALIDO;
    }
}

// --- Lógica de Negócio ---

void carregar_configuracao(const char *caminho_arquivo) {
    char *locale_original = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");

    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (!arquivo) {
        log_evento("CRITICO", "Falha ao abrir arquivo de configuracao: %s", caminho_arquivo);
        perror("ERRO");
        setlocale(LC_NUMERIC, locale_original);
        return;
    }

    Sensor s;
    int campos_lidos;

    // Melhoria no formato: ignorar espaços e garantir leitura robusta
    while ((campos_lidos = fscanf(arquivo, " %d , %[^,] , %f , %f",
                                 &s.id, s.tag, &s.range_min, &s.range_max)) != EOF) {
        if (campos_lidos == 4) {
            inicializar_dados_sensor(&s);
            gerarleituraSensor(&s);
            raiz_sensores = inserirArvoreAVL(raiz_sensores, s);
            log_evento("SUCESSO", "Sensor %s (ID: %d) carregado.", s.tag, s.id);
        } else {
            log_evento("ERRO", "Erro de sintaxe no arquivo de config. Campos: %d", campos_lidos);
        }
    }

    fclose(arquivo);
    setlocale(LC_NUMERIC, locale_original);
    log_evento("SISTEMA", "Configuracao finalizada.");
}

float conversorAD(Sensor s) {
    if (s.leitura_atual == VALOR_INVALIDO) return 0.0f;

    // Regra de 3: (Corrente - 4mA) / 16mA * Delta de Range + Minimo
    float amplitude = s.range_max - s.range_min;
    float proporcao = (s.leitura_atual - MA_MIN) / (MA_MAX - MA_MIN);

    return (proporcao * amplitude) + s.range_min;
}

void gerarleituraSensor(Sensor *s) {
    if (!s) return;

    // Simula leitura 4-20mA
    int sinal_ma = (rand() % 17) + 4;
    s->leitura_atual = (float)sinal_ma;

    float valor_convertido = conversorAD(*s);

    // Log dinâmico de unidade de medida
    const char* unidade = (s->id == 1) ? "°C" : "u.m.";
    log_evento("LEITURA", "TAG: %s | %dmA -> %.2f %s", s->tag, sinal_ma, valor_convertido, unidade);

    // Atualiza histórico circular
    s->historico[s->pos_hist] = valor_convertido;
    s->pos_hist = (s->pos_hist + 1) % TAM_HISTORICO;
}

void atualizarValor(Sensor *s, int mA) {
    if (!s) return;

    s->leitura_atual = (float)mA;
    float valor;

    log_evento("LEITURA", "Valor atualizado manualmente do sensor: (%d) em (%.2f)mA.", s->id, mA);
    if(mA == 0) valor = 0.0;
    else valor = conversorAD(*s);

    // Atualiza histórico circular
    s->historico[s->pos_hist] = valor;
    s->pos_hist = (s->pos_hist + 1) % TAM_HISTORICO;
}

float mediaLeiturasSensor(Sensor *s) {
    if (!s) return 0.0f;

    float soma = 0.0f;
    int contagem = 0;

    for (int i = 0; i < TAM_HISTORICO; i++) {
        if (s->historico[i] != VALOR_INVALIDO) {
            soma += s->historico[i];
            contagem++;
        }
    }

    return (contagem > 0) ? (soma / contagem) : 0.0f;
}

// --- Funções de UI ---

void imprimirInformacoesSensor(Sensor *s) {
    if (!s) {
        printf("\n[Erro]: Sensor nulo.\n");
        return;
    }

    printf("[%s] (ID: %d): ", s->tag, s->id);
    printf("[%.1f a %.1f] ", s->range_min, s->range_max);
    printf("Ultima Leitura: [%.1f] mA ", s->leitura_atual);
    float convertido;
    if(s->leitura_atual == 0) convertido=0;
    else convertido = conversorAD(*s);
    printf("Valor Real: [%.2f].\n", convertido);
}

void imprimirSensor() {
    int id;
    printf("\nDigite o ID do sensor: ");
    if (scanf("%d", &id) != 1) return;

    Sensor *s = buscarSensor(raiz_sensores, id);
    if (s) {
        imprimirInformacoesSensor(s);
    } else {
        printf("\nID %d nao encontrado no sistema.\n", id);
    }
}
