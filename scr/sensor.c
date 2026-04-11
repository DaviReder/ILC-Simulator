#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../include/arvore.h"

// Variável global
No *raiz_sensores = NULL;

void carregar_configuracao(const char *caminho_arquivo) {
    // 1. Necessário pois o arquivo txt lido apresenta float com (.) entre decimais;
    char *locale_original = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");

    // 2. ABERTURA SEGURA
    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        perror("ERRO CRITICO AO ABRIR CONFIG");
        setlocale(LC_NUMERIC, locale_original); // Restaura antes de sair
        return;
    }

    Sensor s;
    int res;

    // 3. LOOP DE LEITURA ROBUSTO
    // O espaço antes de %d pula espaços em branco e quebras de linha residuais
    while ((res = fscanf(arquivo, " %d , %[^,] , %f , %f",
                         &s.id, s.tag, &s.range_min, &s.range_max)) != EOF) {

        if (res == 4) {
            // Inicialização de campos não vindos do arquivo
            s.leitura_atual = 0.0f;
            s.pos_hist = 0;
            for(int i = 0; i < 10; i++) s.historico[i] = -999.0f;

            // Inserção na estrutura de dados
            raiz_sensores = inserirArvoreAVL(raiz_sensores, s);
            printf("[LOG] Sensor %-10s (ID: %d) carregado.\n", s.tag, s.id);
        }
        else if (res > 0) {
            printf("[ERRO] Falha na sintaxe da linha. Campos lidos: %d\n", res);
            break;
        }
    }

    // 4. FINALIZAÇÃO
    fclose(arquivo);
    setlocale(LC_NUMERIC, locale_original);
    printf("[INFO] Processo de configuracao concluido.\n");
}

void imprimirSensor(){
    Sensor *s;
    int id;
    printf("\nProcurar um sensor, digite ID: ");
    scanf("%d", &id);
    s = buscarSensor(raiz_sensores, id);
    if(s){
        printf("\n");
        printf("\nSensor: (%d)", s->id);
        printf("\nTag: (%s)", s->tag);
        printf("\nMin: (%.1f)", s->range_min);
        printf("\nMax: (%.1f)", s->range_max);
        printf("\nLeitura: (%.0f)", s->leitura_atual);
        printf("\n");
    }
    else{
        printf("\nSensor não foi encontrado.\n");
    }
}

float conversorAD(Sensor s){
    if(s.leitura_atual != -999){
        //Convertendo para o valor real medido.
        float dif = s.range_max - s.range_min;
        float fracao = (s.leitura_atual - 4)/16.0;
        return (fracao * dif)+s.range_min;
    }
}

void gerarleituraSensor(Sensor *s){
    if(s){
        //Vou simular uma corrente de saída do sensor: 4-20mA.
        int numGerado = (rand() % 17) + 4;
        s->leitura_atual = numGerado; //Atribui uma corrente como leitura atual.

        //Convertendo para o valor real medido.
        float convertido = conversorAD(*s);
        printf("\nValores gerados:\n Corrente (4-24mA): %d.\nConvertido: %.2f.\n", numGerado, convertido);

        //Adicionando ao historico de medidas:
        s->historico[s->pos_hist] = convertido;
        s->pos_hist++;
        s->pos_hist %= 10;
    }
    else
        printf("\nSensor não encontado!\n");
}

float mediaLeiturasSensor(Sensor *s){
    if(s){
        float media=0.0;
        int j=0;
        for(int i=0; i<10; i++){
            if(s->historico[i] != -999){
                media += s->historico[i];
                j++;
            }
        }
        return (media/j);
    }
    printf("\nSensor não encontrado.\n");
    return 0;

}

void imprimirInformacoesSensor(Sensor *s){
    if(s){
        printf("\nSensor: [ID: %d]  [TAG: %s]", s->id, s->tag);
        printf("\nRange: [%.1f] a [%.1f]", s->range_min, s->range_max);
        printf("\nUltima leitura (mA): [%.0f]", s->leitura_atual);
        printf("\nLeitura real: [%.1f]", conversorAD(*s));
    }
    else
        printf("\nSensor não encontrado.\n");
}
