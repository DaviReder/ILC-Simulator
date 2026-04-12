#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../include/controller.h"

//Thread:
#include <pthread.h>
#include <unistd.h> // Para a função sleep()

// FUNÇÃO DE CADA BIT:
// INT 00000000 00000000 00000000 00000000
// 00000000 00000000 00000000 00000001 -> Presenca
// 00000000 00000000 00000000 00000010 -> Porta fechada
// 00000000 00000000 00000000 00000100 -> Luminosidade
// 00000000 00000000 00000000 00001000 -> Temperatura digital
// 00000000 00000000 00000000 00010000 -> Umidade Digital
// 00000000 00000000 00001111 00000000 -> Analogico luminosidade
// NÃO IMPLEMENTADO:
// 00000000 00000000 01110000 00000000 -> Analogico temperatura/umidade


unsigned int analogLuz(unsigned int saida, float luminosidade){
    unsigned int brilho = 0;

    // Lógica de Dimmer (Inversa: menos luz externa -> mais brilho interno)
    if (luminosidade < 100.0)      brilho = 0x08; // 100% (1000)
    else if (luminosidade < 200.0) brilho = 0x04; // 75%  (0100)
    else if (luminosidade < 270.0) brilho = 0x02; // 50%  (0010)
    else if (luminosidade < 350.0) brilho = 0x01; // 25%  (0001)
    else if (luminosidade < 500.0) brilho = 0x00; // 0%   (0000) - CORRIGIDO AQUI
    else {
        // Acima de 500, desliga o sistema de iluminação completamente
        saida &= ~(1 << 2);             // Desliga Bit Digital
        saida &= ~(0x0F << SHIFT_LUZ);  // Zera Dimmer
        return saida;
    }

    // Ativa o bit digital (Sistema Operacional)
    saida |= (1 << 2);

    // Limpa os 4 bits de brilho (importante para não "sujar" o registrador)
    saida &= ~(0x0F << SHIFT_LUZ);

    // Insere o novo brilho
    saida |= (brilho << SHIFT_LUZ);

    return saida;
}

unsigned int analogTemp(unsigned int saida, float temperatura, float umidade) {
    unsigned int decisoes = 0; // Temporário para montar os 3 bits

    // BIT 0 (Aquecedor): Se < 18°C
    if (temperatura < 17.5) {
        decisoes |= 0x01; // 001
    }
    // BIT 2 (Ar-Condicionado): Se > 26°C
    else if (temperatura > 24.0) {
        decisoes |= 0x04; // 100
    }

    // BIT 1 (Umidificador): Se umidade < 40%
    if (umidade < 40.0) {
        decisoes |= 0x02; // 010
    }

    // --- APLICAÇÃO NO REGISTRADOR ---
    // 1. Limpa a "gaveta" de 3 bits na posição 12
    saida &= ~(MASCARA_CLIMA << SHIFT_CLIMA);
    // 2. Coloca as decisões no elevador e sobe até o andar 12
    saida |= (decisoes << SHIFT_CLIMA);

    return saida;
}

int executaSimulacaoPLC(){
    unsigned int saida=0;

    //Armazena as leituras de cada sensor (temperatura, porta, luminosidade, umidade, presença)
    Sensor *s = buscarSensor(raiz_sensores, 1);
    float temperatura = conversorAD(*s);
    s = buscarSensor(raiz_sensores, 3);
    float luminosidade = conversorAD(*s);
    s = buscarSensor(raiz_sensores, 4);
    float presenca = conversorAD(*s);
    s = buscarSensor(raiz_sensores, 5);
    float umidade = conversorAD(*s);

    // 2. Lógica de Controle
    if (presenca >= 0.55) {
        saida |= 0x01; // Bit 0: Presença ON

        // Chamada das funções especialistas
        saida = analogLuz(saida, luminosidade);
        saida = analogTemp(saida, temperatura, umidade);

        // A porta abre se houver presença (Deixe a porta aberta)
        saida &= ~0x02;
    } else {
        // Sala Vazia: Tudo OFF e Porta FECHADA (Bit 1 em 1)
        saida = 0x02;
    }

    return saida;
}

void* cicloControleAtuadores(void* arg) {
    while(rodando_sistema) {
        Sensor *s ;
        s = buscarSensor(raiz_sensores, 2);
        float umidade = conversorAD(*s);
        s = buscarSensor(raiz_sensores, 1);
        float temperatura = conversorAD(*s);


        // 1. Executa a lógica para decidir o que ligar
        unsigned int status = executaSimulacaoPLC();

        if(status & 0x01){ //Alguem na sala?
            // Porta já está aberta
            int teveControle=0;
            //Controle da temperatura, simulando um atuador ligado
            //Aquecedor:
            if(temperatura <= 20){
                if(s->leitura_atual <= 20 && s->leitura_atual >= 4){
                    s->leitura_atual += 0.1;
                    //Adicionando ao historico de medidas:
                    s->historico[s->pos_hist] = conversorAD(*s);
                    s->pos_hist++;
                    s->pos_hist %= 10;
                    teveControle +=1;
                }
            }
            //Ar condicionado
            else if(temperatura >= 20.5){
                if(s->leitura_atual <= 20 && s->leitura_atual >= 4){
                    s->leitura_atual -= 0.1;
                    //Adicionando ao historico de medidas:
                    s->historico[s->pos_hist] = conversorAD(*s);
                    s->pos_hist++;
                    s->pos_hist %= 10;
                    teveControle +=1;
                }
            }

            s = buscarSensor(raiz_sensores, 5);
            //Umidificador
            if(umidade<=55){
                if(s->leitura_atual <= 20 && s->leitura_atual >= 4){
                    s->leitura_atual += 0.1;
                    //Adicionando ao historico de medidas:
                    s->historico[s->pos_hist] = conversorAD(*s);
                    s->pos_hist++;
                    s->pos_hist %= 10;
                    teveControle +=1;
                }
            }
            if(teveControle == 0){
                printf("\nSala na situação perfeita.\n");
                sleep(25);
            }
            else{
                teveControle = 0;
            }
        }
        else{
            printf("\nNinguem na sala, parando ou deixando de efetuar o controle.\n");
            sleep(15);
        }

        // 3. Espera 5 segundos para o próximo ciclo
        sleep(5);
    }
    return NULL;
}

void monitorarSaidas(unsigned int saida) {
    printf("\n===========================================");
    printf("\n   PAINEL DE CONTROLE DO PLC (DEBUG)");
    printf("\n");
    printf("\n[RAW DATA] Hex: 0x%08X", saida);
    printf("\n");

    unsigned int clima = (saida >> SHIFT_CLIMA) & MASCARA_CLIMA;
    // 1. INTERPRETAÇÃO DOS BITS DIGITAIS (Lógica Discreta)
    printf("\n[DIGITAL]");
    printf("\n  Presenca:      %s", (saida & 0x01) ? "DETECTADA" : "AUSENTE");
    printf("\n  Tranca Porta:  %s", (saida & 0x02) ? "BLOQUEADA (FECHADA)" : "LIBERADA (ABERTA)");
    printf("\n  Luz (Geral):   %s", (saida & 0x04) ? "ON" : "OFF");
    printf("\n  Ar-Condic.:    %s", (clima & 0x04) ? "ON" : "OFF");
    printf("\n  Umidificador:  %s", (clima & 0x02) ? "ON" : "OFF");
    printf("\n  Aquecedor:     %s", (clima & 0x01) ? "ON" : "OFF");

    // 2. INTERPRETAÇÃO DO ANALÓGICO (Byte 1: Luz)
    // Deslocamos 8 bits para a direita para ler apenas a máscara da luz
    unsigned int statusLuz = (saida >> 8) & 0x0F;

    printf("\n\n[ANALOGICO - ILUMINACAO]");
    printf("\n  Intensidade:   ");
    if (statusLuz & 0x08)      printf("100%% (FULL)");
    else if (statusLuz & 0x04) printf("75%%");
    else if (statusLuz & 0x02) printf("50%%");
    else if (statusLuz & 0x01) printf("25%%");
    else                       printf("0%% (OFF)");

    printf("\n===========================================\n");
}
