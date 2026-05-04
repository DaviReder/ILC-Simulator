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

int main() {
    pthread_t thread_id;
    int opcao = 0;

    configurarTerminal();
    carregar_configuracao("data/config.txt");

    if (!raiz_sensores) {
        printf(COR_ERRO "Erro: Sensores nao carregados. Verifique data/config.txt\n" COR_RESET);
        system("pause");
        return -1;
    }

    for (int i = 1; i <= 5; i++) {
        gerarleituraSensor(buscarSensor(raiz_sensores, i));
    }

    pthread_create(&thread_id, NULL, cicloControleAtuadores, NULL);
    system("pause");

    do{
        limparTela();
        exibirCabecalho();
        monitorarPainelPLC(obterRegistroMestre());
        exibirMenuPrincipal();
        scanf("%d", &opcao);

        switch(opcao) {
        case 1:
            printf("\n" COR_TITULO "--- ESTADO ATUAL DOS SENSORES (VALORES REAIS) ---" COR_RESET "\n\n");
            for(int i = 1; i <= 5; i++) {
                imprimirInformacoesSensor(buscarSensor(raiz_sensores, i));
            }
            printf("\n");
            system("pause");
            break;
        case 2:
            printf("\n" COR_AVISO "Injetando ruido nos sensores..." COR_RESET "\n");
            for (int i = 1; i <= 5; i++) {
                gerarleituraSensor(buscarSensor(raiz_sensores, i));
            }
            system("pause");
            break;
        case 3:
            int num;
            printf("\n" COR_AVISO "Digite o ID do sensor: " COR_RESET);
            scanf("%d", &num);
            printf("A média de leituras é: %.2f\n", mediaLeiturasSensor(buscarSensor(raiz_sensores, num)));
            system("pause");
        case 4:
            break;
        default:
            if(opcao != 0){
                printf("\nDigite um número valido!\n");
                break;
            }
            rodando_sistema = 0;
        }
    }while (opcao != 0);

    pthread_join(thread_id, NULL);
    liberarArvore(raiz_sensores);
    printf("\nSistema desligado com seguranca.\n");

    return 0;
}
