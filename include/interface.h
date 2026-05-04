#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

// Cores para usar no printf (Códigos ANSI)
#define COR_RESET   "\033[0m"
#define COR_TITULO  "\033[1;36m" // Ciano
#define COR_AVISO   "\033[1;33m" // Amarelo
#define COR_ERRO    "\033[0;31m" // Vermelho
#define COR_SUCESSO "\033[1;32m" // Verde

// Configurações de Janela e Estética
void configurarTerminal();
void limparTela();

// Componentes do Menu
void exibirCabecalho();
void exibirMenuPrincipal();

#endif
