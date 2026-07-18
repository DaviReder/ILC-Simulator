#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>
#include "include/interface.h"
#include "include/controller.h"
#include "include/sensor.h"
#include "include/arvoreAVL.h"

// Variáveis globais
No *raiz_sensores = NULL;
volatile int rodando_sistema = 1;
unsigned int registro_global_visualizacao = 0;

// ELITE: Mutex global para proteger TODOS os acessos à árvore AVL de sensores
pthread_mutex_t trava_avl;

int main() {
    pthread_t thread_id;
    int opcao = 0;

    configurarTerminal();

    // Inicializa o mutex antes de qualquer operação
    if (pthread_mutex_init(&trava_avl, NULL) != 0) {
        printf(COR_ERRO "Erro fatal: Falha ao inicializar mutex da AVL.\n" COR_RESET);
        return -1;
    }

    carregar_configuracao("data/config.txt");

    if (!raiz_sensores) {
        printf(COR_ERRO "Erro: Sensores nao carregados. Verifique data/config.txt\n" COR_RESET);
        pthread_mutex_destroy(&trava_avl);
        system("pause");
        return -1;
    }

    // ELITE: Protege a inicialização dos sensores com mutex
    pthread_mutex_lock(&trava_avl);
    for (int i = 1; i <= 5; i++) {
        gerarleituraSensor(buscarSensor(raiz_sensores, i));
    }
    pthread_mutex_unlock(&trava_avl);

    // Cria a thread do controlador PLC
    pthread_create(&thread_id, NULL, cicloControleAtuadores, NULL);

    do {
        limparTela();
        exibirCabecalho();
        monitorarPainelPLC(obterRegistroMestre());
        exibirMenuPrincipal();

        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer caso o usuário digite letras para não entrar em loop infinito
            while (getchar() != '\n');
            continue;
        }

        switch(opcao) {
            case 1: {
                printf("\n" COR_TITULO "--- ESTADO ATUAL DOS SENSORES (VALORES REAIS) ---" COR_RESET "\n\n");
                // ELITE: Trava para garantir leitura limpa enquanto o PLC atualiza
                pthread_mutex_lock(&trava_avl);
                for(int i = 1; i <= 5; i++) {
                    imprimirInformacoesSensor(buscarSensor(raiz_sensores, i));
                }
                pthread_mutex_unlock(&trava_avl);
                printf("\n");
                system("pause");
                break;
            }
            case 2: {
                printf("\n" COR_AVISO "Injetando ruido nos sensores..." COR_RESET "\n");
                // ELITE: Trava para injetar ruído de forma síncrona
                pthread_mutex_lock(&trava_avl);
                for (int i = 1; i <= 5; i++) {
                    gerarleituraSensor(buscarSensor(raiz_sensores, i));
                }
                pthread_mutex_unlock(&trava_avl);
                system("pause");
                break;
            }
            case 3: {
                int num;
                printf("\n" COR_AVISO "Digite o ID do sensor: " COR_RESET);
                scanf("%d", &num);

                // ELITE: Trava a busca e o cálculo da média para evitar SegFault se a AVL rebalancear
                pthread_mutex_lock(&trava_avl);
                Sensor *s = buscarSensor(raiz_sensores, num);
                if (s != NULL) {
                    printf("A média de leituras é: %.2f\n", mediaLeiturasSensor(s));
                } else {
                    printf(COR_ERRO "Sensor com ID %d não encontrado.\n" COR_RESET, num);
                }
                pthread_mutex_unlock(&trava_avl);

                system("pause");
                break; // ELITE: BUG CORRIGIDO! Antes "escorregava" para o case 4.
            }
            case 4:
                // Espaço para futuras implementações (PID, etc)
                break;
            default:
                if (opcao != 0) {
                    printf("\nDigite um número valido!\n");
                    system("pause");
                    break;
                }
                rodando_sistema = 0;
        }
    } while (opcao != 0);

    // Sinaliza desligamento e aguarda o fechamento da thread do PLC
    pthread_join(thread_id, NULL);

    // Liberação final segura
    pthread_mutex_lock(&trava_avl);
    liberarArvore(raiz_sensores);
    pthread_mutex_unlock(&trava_avl);

    pthread_mutex_destroy(&trava_avl);
    printf("\nSistema desligado com segurança.\n");

    return 0;
}
