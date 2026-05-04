#ifndef CONTROLLER_H
#define CONTROLLER_H

// =========================================================================
// MAPA DE REGISTRADORES (MEMÓRIA DO PLC)
// =========================================================================
#define BIT_AQUECEDOR     (1 << 0)  // 0x01
#define BIT_AR_COND       (1 << 1)  // 0x02
#define BIT_UMIDIFICADOR  (1 << 2)  // 0x04
#define BIT_BOMBA_AGUA    (1 << 3)  // 0x08
#define SHIFT_LUZ         4         // Bits 4-7
#define MASCARA_DIMMER    0x0F
#define BIT_PRESENCA      (1 << 8)
#define BIT_PORTA_TRANCA  (1 << 31)

// =========================================================================
// PARÂMETROS DE CONTROLE (SETPOINTS E HISTERESE)
// =========================================================================
#define SETPOINT_TEMP     22.0f
#define HISTERESE_TEMP    1.5f
#define SETPOINT_UMID     55.0f
#define HISTERESE_UMID    5.0f

// =========================================================================
// PROTÓTIPOS
// =========================================================================
unsigned int calcularLogicaControle(float t_atual, float u_atual, float luz_atual, int presenca);
void* cicloControleAtuadores(void* arg);
unsigned int obterRegistroMestre();
void monitorarPainelPLC(unsigned int registro);

#endif
