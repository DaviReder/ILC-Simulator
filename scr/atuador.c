#include <stdio.h>
#include "../include/atuador.h"
#include "../include/controller.h"
#include "../include/arvoreAVL.h"

extern No *raiz_sensores;

void processarEfeitoAtuadores(unsigned int registro) {
    Sensor *s_temp = buscarSensor(raiz_sensores, 1);
    Sensor *s_umid = buscarSensor(raiz_sensores, 5);

    // --- PROCESSAMENTO DE TEMPERATURA ---
    if (s_temp) {
        if (registro & BIT_AQUECEDOR) {
            s_temp->leitura_atual += 0.08f;
        }
        else if (registro & BIT_AR_COND) {
            s_temp->leitura_atual -= 0.12f;
        }

        // Clamp de Segurança (4-20mA)
        if (s_temp->leitura_atual < 4.0f)  s_temp->leitura_atual = 4.0f;
        if (s_temp->leitura_atual > 20.0f) s_temp->leitura_atual = 20.0f;
    }

    // --- PROCESSAMENTO DE UMIDADE ---
    if (s_umid) {
        // O Umidificador aumenta a umidade
        if (registro & BIT_UMIDIFICADOR) {
            s_umid->leitura_atual += 0.10f;
        }

        // Efeito colateral: O Ar-Condicionado retira umidade do ar
        if (registro & BIT_AR_COND) {
            s_umid->leitura_atual -= 0.05f;
        }

        // Clamp de Segurança (4-20mA)
        if (s_umid->leitura_atual < 4.0f)  s_umid->leitura_atual = 4.0f;
        if (s_umid->leitura_atual > 20.0f) s_umid->leitura_atual = 20.0f;
    }
}
