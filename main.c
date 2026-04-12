#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <pthread.h>
#include <windows.h>
#include "include/arvore.h"
#include "include/controller.h"

// Protótipos de funções locais
void configurarTerminal();
void limparTela();
int inicializaSistema();
volatile int rodando_sistema = 1;

int main() {
    pthread_t thread_id;
    int id_busca, opcao;
    Sensor *s;

    // 1. Configurações Iniciais de Janela e Sistema
    configurarTerminal();
    if (!inicializaSistema()) {
        printf("\033[0;31mFalha ao iniciar sistema. Verifique os arquivos de config.\033[0m\n");
        system("pause");
        return -1;
    }

    // 2. Dispara a Thread de Controle (Background)
    pthread_create(&thread_id, NULL, cicloControleAtuadores, NULL);
    system("pause");

    do {
        limparTela();

        // --- CABEÇALHO ESTÁTICO ---
        printf("\033[1;36m"); // Cor Ciano Negrito
        printf("======================================================================\n");
        printf("   PLC INDUSTRIAL SIMULATOR v1.0 | STATUS EM TEMPO REAL               \n");
        printf("======================================================================\n\033[0m");

        // --- PAINEL DE MONITORAMENTO (Sempre visível no topo) ---
        unsigned int saida_atual = executaSimulacaoPLC();
        monitorarSaidas(saida_atual);

        // --- MENU DE INTERAÇÃO ---
        printf("\n\033[1;33m>>>> MENU DE OPERACAO <<<<\033[0m\n");
        printf("[1] Listar Sensores (Detalhado)    [3] Media de Leitura por ID\n");
        printf("[2] Aleatorizar Sensores (Ruido)   [4] Refresh Manual do Painel\n");
        printf("[0] Sair e gerar LOG Final\n");
        printf("\033[1;32mCOMANDO > \033[0m");

        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n'); // Limpa buffer em caso de entrada inválida
            continue;
        }

        // --- PROCESSAMENTO DAS OPÇÕES ---
        switch(opcao) {
            case 1:
                printf("\n\033[1;34m[INFO] Varrendo Arvore AVL de Sensores...\033[0m\n");
                for(int i = 1; i <= 5; i++) {
                    imprimirInformacoesSensor(buscarSensor(raiz_sensores, i));
                    printf("-------------------------------------------");
                }
                printf("\n");
                system("pause");
                break;

            case 2:
                for (int i = 1; i <= 5; i++) {
                    s = buscarSensor(raiz_sensores, i);
                    if (s) gerarleituraSensor(s);
                }
                printf("\n\033[0;32mSensores aleatorizados com sucesso!\033[0m\n");
                Sleep(1500); // Pequena pausa para feedback visual
                break;

            case 3:
                printf("\nID do Sensor para calculo de media: ");
                scanf("%d", &id_busca);
                Sensor *s_alvo = buscarSensor(raiz_sensores, id_busca);
                if (s_alvo) {
                    float media = mediaLeiturasSensor(s_alvo);
                    printf("\n\033[1;35m[RESULTADO] Media (ID %d): %.2f u.m.\033[0m\n", id_busca, media);
                } else {
                    printf("\n\033[0;31m[ERRO] Sensor ID %d nao encontrado.\033[0m\n", id_busca);
                }
                system("pause");
                break;

            case 4:
                // Apenas volta para o início do loop para redesenhar
                break;

            case 0:
                printf("\n\033[1;33mFinalizando threads e gerando LOG...\033[0m\n");
                rodando_sistema = 0;
                // 2. Esperar a thread terminar de verdade.
                pthread_join(thread_id, NULL);

                // Aqui você chamaria sua função de LOG:
                // gerarRelatorioLog("data/log_final.txt");
                break;

            default:
                printf("\n\033[0;31mOpcao invalida.\033[0m\n");
                Sleep(1000);
        }

    } while(opcao != 0);

    printf("\nSistema encerrado com sucesso.\n");
    Sleep(1500);
    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES ---

void configurarTerminal() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND console = GetConsoleWindow();

    // 1. Título
    SetConsoleTitle("PLC Industrial Simulator v1.0 - HD Monitor");

    // 2. Definir o tamanho do BUFFER antes da janela
    // Diminuí um pouco os valores para evitar conflito com fontes grandes
    COORD coord;
    coord.X = 120; // 120 colunas de texto
    coord.Y = 40;  // 40 linhas de texto
    SetConsoleScreenBufferSize(hConsole, coord);

    // 3. Ajustar a JANELA física
    // O MoveWindow agora vem depois para garantir que o buffer já existe
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 1280, 720, TRUE);

    // 4. Travar o estilo (Sem redimensionar)
    long style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);

    // 5. Cursor invisível
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void limparTela() {
    // Se o \033[H estiver deixando rastro, use este comando duplo:
    // \033[H (Home) + \033[2J (Limpa a tela inteira mas mantém o cursor no topo)
    printf("\033[H\033[2J");
}

int inicializaSistema() {
    srand(time(NULL));
    setlocale(LC_ALL, "Portuguese");

    carregar_configuracao("data/config.txt");

    if (raiz_sensores == NULL) return 0;

    // Leitura inicial de estabilização
    for (int i = 1; i <= 5; i++) {
        Sensor *s = buscarSensor(raiz_sensores, i);
        if (s) gerarleituraSensor(s);
    }
    return 1;
}
