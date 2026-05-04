#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/codificar.h"

// ====================================================
// =========== FUNCOES AUXILIARES DE BITS   ===========
// ====================================================

// Retorna o valor do bit na posição i de um byte
unsigned int testa_bit(unsigned char byte, int i) {
    unsigned char mascara = (1 << i);
    return byte & mascara;
}

// ====================================================
// =========== FUNCOES TABELA DE FREQUENCIA ===========
// ====================================================

void inicializa_com_zero(unsigned int vet[]){
    for(int i=0; i<TAM; i++){
        vet[i] = 0;
    }
}

void preenche_tabela(unsigned char texto[], unsigned int tabela[]){
    int i = 0;
    while(texto[i] != '\0'){
        tabela[texto[i]]++;
        i++;
    }
}

void imprime_tabela(unsigned int tabela[]){
    printf("\nTabela de Frequência\n");
    for(int i=0; i<TAM; i++){
        if(tabela[i] > 0){
            printf("[%d]: %d = '%c'\n", i, tabela[i], i);
        }
    }
}

// ======================================================
// =========== FUNCOES PARA LISTA 'ENCADEADA' ===========
// ======================================================

void criar_lista(Lista *lista){
    lista->inicio = NULL;
    lista->tam = 0;
}

void inserir_ordenado(Lista *lista, No *no){
    No *aux;
    if(lista->inicio == NULL){
        lista->inicio = no;
    }
    else if(no->frequencia < lista->inicio->frequencia){
        no->prox = lista->inicio;
        lista->inicio = no;
    }
    else{
        aux = lista->inicio;
        while(aux->prox && aux->prox->frequencia <= no->frequencia){
            aux = aux->prox;
        }
        no->prox = aux->prox;
        aux->prox = no;
    }
    lista->tam++;
}

void preencher_lista(unsigned int tab[], Lista *lista){
    No *novo;
    for(int i=0; i<TAM; i++){
        if(tab[i] > 0){
            novo = malloc(sizeof(No));
            if(novo){
                novo->caracter = i;
                novo->dir = NULL;
                novo->esq = NULL;
                novo->frequencia = tab[i];
                novo->prox = NULL;
                inserir_ordenado(lista, novo);
            }
            else{
                printf("\nErro ao alocar memoria: 'preencher_lista'.");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista){
    No *aux = lista->inicio;
    printf("\nImprimindo Lista: Tamanho: %d\n", lista->tam);
    while(aux){
        printf("Caracter: %c, Frequência: %u\n", aux->caracter, aux->frequencia);
        aux = aux->prox;
    }
    printf("\n");
}

// ======================================================
// =========== FUNCOES PARA ARVORE DE HUFFMAN ===========
// ======================================================

No* remover_inicio(Lista *lista){
    No *aux = NULL;
    if(lista->inicio){
        aux = lista->inicio;
        lista->inicio = aux->prox;
        aux->prox = NULL;
        lista->tam--;
    }
    return aux;
}

No* montar_arvore(Lista *lista){
    No *primeiro, *segundo, *novo;
    while(lista->tam > 1){
        primeiro = remover_inicio(lista);
        segundo = remover_inicio(lista);
        novo = malloc(sizeof(No));
        if(novo){
            novo->caracter = '+';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->prox = NULL;
            inserir_ordenado(lista, novo);
        }
        else{
            printf("\nErro ao alocar memória: 'montar_arvore'.");
            break;
        }
    }
    return lista->inicio;
}

// Limpa a árvore da memória (recursivo)
void liberar_arvore(No *raiz) {
    if(raiz == NULL) return;
    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);
    free(raiz);
}

void imprimir_arvore(No *raiz, int tam){
    if(raiz->esq == NULL && raiz->dir == NULL){
        printf("Folha: %c\tAltura: %d\n", raiz->caracter, tam);
    }
    else{
        imprimir_arvore(raiz->esq, tam+1);
        imprimir_arvore(raiz->dir, tam+1);
    }
}

// ======================================================
// =========== MONTAGEM DO DICIONARIO HUFFMAN ===========
// ======================================================

int altura_arvore(No *raiz){
    int esq, dir;
    if(raiz == NULL) return -1;
    esq = altura_arvore(raiz->esq) + 1;
    dir = altura_arvore(raiz->dir) + 1;
    return (esq > dir) ? esq : dir;
}

char** aloca_dicionario(int colunas){
    char **dicionario = malloc(sizeof(char*) * TAM);
    for(int i=0; i<TAM; i++){
        dicionario[i] = calloc(colunas, sizeof(char));
    }
    return dicionario;
}

// Limpa o dicionário da memória
void liberar_dicionario(char **dicionario) {
    for(int i = 0; i < TAM; i++) free(dicionario[i]);
    free(dicionario);
}

void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas){
    char esquerda[colunas], direita[colunas];
    if(raiz->dir == NULL && raiz->esq == NULL){
        strcpy(dicionario[raiz->caracter], caminho);
    }
    else{
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);
        strcat(esquerda, "0");
        strcat(direita, "1");
        gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->dir, direita, colunas);
    }
}

void imprimir_dicionario(char **dicionario){
    printf("\nDicionario de Huffman\n");
    for(int i=0; i<TAM; i++){
        if(strlen(dicionario[i]) > 0)
            printf("%3d: %s\n", i, dicionario[i]);
    }
}

// ===================================
// =========== CODIFICAÇÃO ===========
// ===================================

int calc_tamanho_string(char **dicionario, unsigned char *texto){
    int i=0, tam=0;
    while(texto[i] != '\0'){
        tam += strlen(dicionario[texto[i]]);
        i++;
    }
    return tam+1;
}

char* codificar(char **dicionario, unsigned char *texto){
    int i=0, tam = calc_tamanho_string(dicionario, texto);
    char *codigo = calloc(tam, sizeof(char));
    while(texto[i] != '\0'){
        strcat(codigo, dicionario[texto[i]]);
        i++;
    }
    return codigo;
}

// =========================================
// =========== MANIPULAR ARQUIVO ===========
// =========================================

void compactar(const char *nome_saida, unsigned char codificado[], unsigned int tabela[]){
    FILE *arquivo = fopen(nome_saida, "wb");
    if(arquivo == NULL) return;

    int total_registros = 0;
    for(int i = 0; i < TAM; i++) if(tabela[i] > 0) total_registros++;

    fwrite(&total_registros, sizeof(int), 1, arquivo);

    long pos_lixo = ftell(arquivo);
    unsigned char lixo = 0;
    fwrite(&lixo, sizeof(unsigned char), 1, arquivo);

    for(int i = 0; i < TAM; i++) {
        if(tabela[i] > 0) {
            unsigned char c = (unsigned char)i;
            fwrite(&c, sizeof(unsigned char), 1, arquivo);
            fwrite(&tabela[i], sizeof(unsigned int), 1, arquivo);
        }
    }

    int i = 0, j = 7;
    unsigned char mascara, byte = 0;

    while(codificado[i] != '\0') {
        mascara = 1;
        if(codificado[i] == '1') {
            mascara = mascara << j;
            byte = byte | mascara;
        }
        j--;
        if(j < 0) {
            fwrite(&byte, sizeof(unsigned char), 1, arquivo);
            byte = 0;
            j = 7;
        }
        i++;
    }

    if(j != 7) {
        fwrite(&byte, sizeof(unsigned char), 1, arquivo);
        lixo = j + 1;
        fseek(arquivo, pos_lixo, SEEK_SET);
        fwrite(&lixo, sizeof(unsigned char), 1, arquivo);
    }

    fclose(arquivo);
    printf("\nArquivo compactado com sucesso!");
}

// ================================================
// =========== ENTRADA DE TEXTO POR LOG ===========
// ================================================

int descobrir_tamanho(const char *caminho){
    FILE *arquivo = fopen(caminho, "r");
    if(arquivo == NULL) return 0;
    fseek(arquivo, 0, SEEK_END);
    int tam = ftell(arquivo);
    fclose(arquivo);
    return tam;
}

void ler_texto(const char *caminho, unsigned char *frase){
    FILE *arquivo = fopen(caminho, "r");
    if(arquivo == NULL) return;
    int i = 0, letra;
    while((letra = fgetc(arquivo)) != EOF){
        frase[i++] = (unsigned char)letra;
    }
    frase[i] = '\0';
    fclose(arquivo);
}

// =====================================================
// =========== EXECUTAR FLUXO DE COMPACTACAO ===========
// =====================================================

void executar_compactacao_autonoma(const char *arquivo_origem, const char *arquivo_destino) {
    int tam = descobrir_tamanho(arquivo_origem);
    if(tam == 0) return;

    unsigned char *frase = calloc(tam + 1, sizeof(unsigned char));
    if (!frase) return;
    ler_texto(arquivo_origem, frase);

    unsigned int tabela_frequencia[TAM];
    inicializa_com_zero(tabela_frequencia);
    preenche_tabela(frase, tabela_frequencia);

    Lista lista;
    criar_lista(&lista);
    preencher_lista(tabela_frequencia, &lista);
    No *arvore = montar_arvore(&lista);

    int colunas = altura_arvore(arvore) + 1;
    char **dicionario = aloca_dicionario(colunas);
    gerar_dicionario(dicionario, arvore, "", colunas);

    char *codigo = codificar(dicionario, frase);

    // Usa o nome de saída dinâmico
    compactar(arquivo_destino, codigo, tabela_frequencia);

    free(frase);
    free(codigo);
    liberar_dicionario(dicionario);
    liberar_arvore(arvore);
}
