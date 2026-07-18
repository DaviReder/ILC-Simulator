#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/decodificar.h"

// ================================================
// =========== DECODIFICAR POR PASSAGEM ===========
// ================================================

// Mantida a assinatura idêntica à sua original: ordem dos parâmetros e tipo 'No'
char* decodificar(unsigned char texto[], No *raiz){
    if (!raiz || !texto) return NULL;

    No *aux = raiz;

    // Aloca memória inicial limpa
    int tam_max = strlen((char*)texto);
    char *decodificado = calloc(tam_max + 1, sizeof(char));
    if (!decodificado) return NULL;

    int idx_escrita = 0;

    for(int i = 0; texto[i] != '\0'; i++){
        if(texto[i] == '0'){
            aux = aux->esq;
        }
        else{
            aux = aux->dir;
        }

        if(aux->esq == NULL && aux->dir == NULL){
            decodificado[idx_escrita++] = aux->caracter; // Inserção ultra rápida O(1)
            aux = raiz;
        }
    }

    decodificado[idx_escrita] = '\0'; // Finaliza a string com segurança

    // Ajusta o bloco de memória para o tamanho exato que foi preenchido
    char *ajustado = realloc(decodificado, (idx_escrita + 1) * sizeof(char));
    if (ajustado) {
        decodificado = ajustado;
    }

    return decodificado;
}

// ==============================================================
// =========== DECODIFICAR POR MANIPULAÇÃO DE ARQUIVO ===========
// ==============================================================

void descompactar(char *nome_compactado, char *nome_saida) {
    FILE *entrada = fopen(nome_compactado, "rb");
    FILE *saida = fopen(nome_saida, "wb");

    if(!entrada || !saida) {
        if(entrada) fclose(entrada);
        if(saida) fclose(saida);
        printf("\nErro ao abrir os arquivos para descompactação!\n");
        return;
    }

    // 1. Ler o Header
    int total_registros;
    unsigned char lixo;
    fread(&total_registros, sizeof(int), 1, entrada);
    fread(&lixo, sizeof(unsigned char), 1, entrada);

    unsigned int tabela_recuperada[TAM];
    for(int i = 0; i < TAM; i++) tabela_recuperada[i] = 0;

    for(int i = 0; i < total_registros; i++) {
        unsigned char c;
        unsigned int f;
        fread(&c, sizeof(unsigned char), 1, entrada);
        fread(&f, sizeof(unsigned int), 1, entrada);
        tabela_recuperada[c] = f;
    }

    // 2. Reconstruir a Árvore
    Lista lista;
    criar_lista(&lista);
    preencher_lista(tabela_recuperada, &lista);
    No *raiz = montar_arvore(&lista);
    No *aux = raiz;

    // 3. Decodificar e Salvar no Arquivo
    unsigned char byte_atual, proximo_byte;

    // Lógica robusta original mantida intacta (.esq e .dir)
    if(fread(&byte_atual, sizeof(unsigned char), 1, entrada)) {
        while(fread(&proximo_byte, sizeof(unsigned char), 1, entrada)) {
            for(int i = 7; i >= 0; i--) {
                if(testa_bit(byte_atual, i)) aux = aux->dir;
                else aux = aux->esq;

                if(aux->dir == NULL && aux->esq == NULL) {
                    fputc(aux->caracter, saida);
                    aux = raiz;
                }
            }
            byte_atual = proximo_byte;
        }

        // Processar o último byte considerando o lixo
        for(int i = 7; i >= (int)lixo; i--) {
            if(testa_bit(byte_atual, i)) aux = aux->dir;
            else aux = aux->esq;

            if(aux->dir == NULL && aux->esq == NULL) {
                fputc(aux->caracter, saida);
                aux = raiz;
            }
        }
    }

    // --- LIMPEZA CRÍTICA ---
    liberar_arvore(raiz);
    fclose(entrada);
    fclose(saida);

    printf("\nDescompactação concluída! Arquivo '%s' gerado.\n", nome_saida);
}
