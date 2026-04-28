#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../include/arvore.h"

No *raiz_sensores = NULL;

void carregar_configuracao(const char *caminho_arquivo) {
    char *locale_original = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");

    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        log_evento("CRITICO", "Arquivo  (Linha 17, arvoreAVL.c)");
        perror("ERRO CRITICO AO ABRIR CONFIG");
        setlocale(LC_NUMERIC, locale_original); // Restaura antes de sair
        return;
    }

    Sensor s;
    int res;

    while ((res = fscanf(arquivo, " %d , %[^,] , %f , %f",
                         &s.id, s.tag, &s.range_min, &s.range_max)) != EOF) {

        if (res == 4) {
            // Inicialização de campos não vindos do arquivo
            s.leitura_atual = 0.0f;
            s.pos_hist = 0;
            for(int i = 0; i < 10; i++) s.historico[i] = -999.0f;

            // Inserção na estrutura de dados
            raiz_sensores = inserirArvoreAVL(raiz_sensores, s);
            log_evento("SUCESSO", "Sensor %-10s (ID: %d) carregado.", s.tag, s.id);
        }
        else if (res > 0) {
            log_evento("ERRO", "Falha na sintaxe (Linha 42, sensor.c). Campos lidos: %d.", res);
            break;
        }
    }

    fclose(arquivo);
    setlocale(LC_NUMERIC, locale_original);
    log_evento("SUCESSO", "Processo de configuracao concluido.", s.tag, s.id);
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
        float dif = s.range_max - s.range_min;
        float fracao = (s.leitura_atual - 4)/16.0;
        return (fracao * dif)+s.range_min;
    }
    return 0;
}

void gerarleituraSensor(Sensor *s){
    if(s){
        int numGerado = (rand() % 17) + 4;
        s->leitura_atual = numGerado;

        float convertido = conversorAD(*s);
        log_evento("ALEATORIZADO", "TAG: %s | Corrente: %dmA | Valor: %.2f %s", s->tag, numGerado, convertido, (s->id == 1 ? "°C" : "u.m."));
        s->historico[s->pos_hist] = convertido;
        s->pos_hist++;
        s->pos_hist %= 10;
    }
    else{
        log_evento("WARNING", "Sensor não encontrado (Linha 98, sensor.c).");
        printf("\nSensor não encontado!\n");
    }
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
    log_evento("WARNING", "Sensor não encontrado (Linha 114, sensor.c).");
    printf("\nSensor não encontrado.\n");
    return 0;

}

void imprimirInformacoesSensor(Sensor *s){
    if(s){
        printf("\n[%s]: %d", s->tag, s->id);
        printf("\n[Range]: %.0f a %.0f", s->range_min, s->range_max);
        printf("\n[Leitura (mA)]: %.1f", s->leitura_atual);
        printf("\n[Medida real]: [%.1f]\n", conversorAD(*s));
    }
    else
        printf("\nSensor não encontrado.\n");
}
