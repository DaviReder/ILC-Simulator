#include <stdio.h>
#include <windows.h>
#include "../include/interface.h"

void limparTela() {
    // \033[H  -> Move o cursor para a linha 1, coluna 1
    // \033[J  -> Limpa do cursor até o final da tela (apaga rastro)
    printf("\033[H\033[J");
    fflush(stdout); // Força o Windows a desenhar imediatamente
}

void configurarTerminal() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 1. Desabilitar o modo de rolagem forçando o buffer a ser do tamanho da janela
    COORD coord = {110, 35}; // Largura e Altura
    SetConsoleScreenBufferSize(hConsole, coord);

    // 2. Esconder o cursor (aquela barrinha piscando que estraga o visual)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    SetConsoleTitle("PLC Simulator - Monitor Ativo");
}

void exibirCabecalho() {
    printf(COR_TITULO);
    printf("========================================================================\n");
    printf("=           PLC INDUSTRIAL SIMULATOR | STATUS EM TEMPO REAL            =\n");
    printf("========================================================================\n");
    printf(COR_RESET);
}

void exibirMenuPrincipal() {
    printf("\n" COR_AVISO ">>>> MENU DE OPERACAO <<<<" COR_RESET "\n");
    printf("[1] Listar Sensores (Detalhado)    [3] Media de Leitura por ID\n");
    printf("[2] Aleatorizar Sensores (Ruido)   [4] Refresh Manual do Painel\n");
    printf("[0] Sair e gerar LOG Final\n");
    printf(COR_SUCESSO "\nCOMANDO > " COR_RESET);
}
