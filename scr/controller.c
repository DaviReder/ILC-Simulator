#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/controller.h"
#include "../include/arvoreAVL.h"
#include "../include/sensor.h"
#include "../include/log.h"
#include "../include/interface.h"
#include "../include/atuador.h"

extern pthread_mutex_t trava_avl;
extern No *raiz_sensores;
extern volatile int rodando_sistema;
static unsigned int registro_saida_mestre = 0;

unsigned int calcularLogicaControle(float t_atual, float u_atual, float luz_atual, int presenca) {

    // 1. Lógica de Segurança (Presença)
    if (!presenca) {
        // Se não há ninguém, desliga tudo e ativa a tranca da porta (Bit 31)
        registro_saida_mestre = BIT_PORTA_TRANCA;
        return registro_saida_mestre;
    }

    // 2. Controle de Temperatura com Histerese e Intertravamento
    // Logica para o Aquecedor
    if (t_atual < (SETPOINT_TEMP - HISTERESE_TEMP)) {
        registro_saida_mestre |= BIT_AQUECEDOR;
        registro_saida_mestre &= ~BIT_AR_COND; // Segurança: Nunca liga os dois juntos
    }
    else if (t_atual >= SETPOINT_TEMP) {
        registro_saida_mestre &= ~BIT_AQUECEDOR;
    }

    // Logica para o Ar-Condicionado
    if (t_atual > (SETPOINT_TEMP + HISTERESE_TEMP)) {
        registro_saida_mestre |= BIT_AR_COND;
        registro_saida_mestre &= ~BIT_AQUECEDOR; // Segurança: Intertravamento
    }
    else if (t_atual <= SETPOINT_TEMP) {
        registro_saida_mestre &= ~BIT_AR_COND;
    }

    // 3. Controle de Umidade
    if (u_atual < (SETPOINT_UMID - HISTERESE_UMID)) {
        registro_saida_mestre |= BIT_UMIDIFICADOR;
    }
    else if (u_atual >= SETPOINT_UMID) {
        registro_saida_mestre &= ~BIT_UMIDIFICADOR;
    }

    // 4. Lógica do Dimmer de Iluminação (Bits 4-7)
    unsigned int intensidade = 0;
    if (luz_atual < 150.0f)      intensidade = 0x0F; // 100% (F em hexa)
    else if (luz_atual < 400.0f) intensidade = 0x08; // 50%
    else                         intensidade = 0x00; // 0%

    // Limpa apenas o campo do dimmer (bits 4-7) e insere o novo valor
    registro_saida_mestre &= ~(MASCARA_DIMMER << SHIFT_LUZ);
    registro_saida_mestre |= (intensidade << SHIFT_LUZ);

    // 5. Bits de Status Final
    registro_saida_mestre |= BIT_PRESENCA;
    registro_saida_mestre &= ~BIT_PORTA_TRANCA; // Destranca pois há presença

    return registro_saida_mestre;
}

void* cicloControleAtuadores(void* arg) {
    log_evento("SISTEMA", "Controlador em execução...");

    while(rodando_sistema) {
        // LOCK: Bloqueia a AVL para realizar a busca e manipulação dos nós com segurança concorrente
        pthread_mutex_lock(&trava_avl);

        Sensor *s_temp = buscarSensor(raiz_sensores, 1);
        Sensor *s_porta  = buscarSensor(raiz_sensores, 2);
        Sensor *s_luz  = buscarSensor(raiz_sensores, 3);
        Sensor *s_pres = buscarSensor(raiz_sensores, 4);
        Sensor *s_umid = buscarSensor(raiz_sensores, 5);

        if (s_temp && s_luz && s_pres && s_umid) {
            // Converte os valores atuais para Unidades de Engenharia
            float t = conversorAD(*s_temp);
            float l = conversorAD(*s_luz);
            float u = conversorAD(*s_umid);
            float p = conversorAD(*s_pres);

            // A ÚNICA chamada de decisão
            unsigned int novo_status = calcularLogicaControle(t, u, l, (p > 0.5f));

            // Aplica a física (Aumenta/Diminui mA baseado nos bits de novo_status)
            processarEfeitoAtuadores(novo_status);

            // Se o bit de presença estiver desligado no registro, zeramos os sensores
            if (!(novo_status & BIT_PRESENCA)) {
                atualizarValor(s_temp, 0);
                atualizarValor(s_porta, 0);
                atualizarValor(s_umid, 0);
                atualizarValor(s_luz, 0);
            }
        }

        // UNLOCK: Libera a árvore imediatamente após o término do processamento do ciclo
        pthread_mutex_unlock(&trava_avl);

        sleep(5); // Mantém o tempo de varredura (Scan Time) do PLC de forma suave
    }
    return NULL;
}

unsigned int obterRegistroMestre() {
    return registro_saida_mestre;
}

void monitorarPainelPLC(unsigned int registro) {
    unsigned int brilho = (registro >> SHIFT_LUZ) & MASCARA_DIMMER;
    printf("\n" COR_TITULO "================ REGISTRADOR DE SAIDA PLC (0x%08X) =================" COR_RESET, registro);

    printf("\n[STATUS]:  Ocupacao: %s | Seguranca: %s",
           (registro & BIT_PRESENCA) ? "SIM" : "NAO",
           (registro & BIT_PORTA_TRANCA) ? "TRANCADA" : "LIBERADA");

    printf("\n[ATUADORES]: Aquec: [%s] | Ar: [%s] | Umid: [%s]",
           (registro & BIT_AQUECEDOR) ? "LIGADO" : "DESL",
           (registro & BIT_AR_COND) ? "LIGADO" : "DESL",
           (registro & BIT_UMIDIFICADOR) ? "LIGADO" : "DESL");

    printf("\n[ILUMINACAO]: Intensidade Dimmer: %d de 15", brilho);
    printf("\n========================================================================\n");
}
